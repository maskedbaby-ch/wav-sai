<%--********************************************************************************
* CruiseControl, a Continuous Integration Toolkit
* Copyright (c) 2001, ThoughtWorks, Inc.
* 200 E. Randolph, 25th Floor
* Chicago, IL 60601 USA
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions
* are met:
*
*     + Redistributions of source code must retain the above copyright
*       notice, this list of conditions and the following disclaimer.
*
*     + Redistributions in binary form must reproduce the above
*       copyright notice, this list of conditions and the following
*       disclaimer in the documentation and/or other materials provided
*       with the distribution.
*
*     + Neither the name of ThoughtWorks, Inc., CruiseControl, nor the
*       names of its contributors may be used to endorse or promote
*       products derived from this software without specific prior
*       written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
* "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
* LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
* A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE REGENTS OR
* CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
* EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
* PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
* PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
* LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
* NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
* SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
********************************************************************************--%>
<%@ page errorPage="/error.jsp" contentType="text/html; charset=UTF-8"%>
<%@ taglib uri="/WEB-INF/cruisecontrol-jsp11.tld" prefix="cruisecontrol"%>
<%@ page import="net.sourceforge.cruisecontrol.*" %>
<%@ page import="java.io.IOException" %>
<%@ page import="java.net.InetAddress" %>
<%@ page import="java.net.URL" %>
<%@ page import="java.text.DateFormat" %>
<%@ page import="java.io.File" %>
<%@ page import="java.util.Arrays" %>
<%@ page import="java.text.ParseException" %>
<%@ page import="java.io.BufferedReader" %>
<%@ page import="java.io.FileReader" %>
<%@ page import="java.util.HashMap" %>
<%@ page import="java.util.Date" %>
<%@ page import="java.util.Comparator" %>
<%@ page import="java.util.Map" %>


<%
  final DateFormat dateTimeFormat = DateFormat.getDateTimeInstance(DateFormat.SHORT, DateFormat.SHORT, request.getLocale());
  final DateFormat dateOnlyFormat = DateFormat.getDateInstance(DateFormat.SHORT, request.getLocale());
  final DateFormat timeOnlyFormat = DateFormat.getTimeInstance(DateFormat.SHORT, request.getLocale());

  final Date now = new Date();
  final String dateNow = dateTimeFormat.format(now);
%>

<%


  class SortableStatus implements Comparable {
    private ProjectState state;
    private String importance;
    private int sortOrder;

    public SortableStatus(ProjectState state, String importance, int sortOrder) {
      this.state = state;
      this.importance = importance;
      this.sortOrder = sortOrder;
    }

    public String toString() {
      return state != null ? state.getName() : "?";
    }

    public int getSortOrder() {
      return sortOrder;
    }

    public int compareTo(Object other) {
      SortableStatus that = (SortableStatus) other;
      return this.sortOrder - that.sortOrder;
    }

    public String getImportance() {
      return importance;
    }
  }

  class StatusCollection {
    private Map statuses = new HashMap();
    private SortableStatus unknown = new SortableStatus(null, "dull", -1);

    public void add(ProjectState state, String importance) {
      statuses.put(state.getDescription(), new SortableStatus(state, importance, statuses.size()));
    }

    public SortableStatus get(String statusDescription) {
      Object status = statuses.get(statusDescription);
      if (status != null) {
        return (SortableStatus) status;
      }
      return unknown;
    }
  }

%>

<%
  final StatusCollection statuses = new StatusCollection();
  statuses.add(ProjectState.PUBLISHING, "important");
  statuses.add(ProjectState.MODIFICATIONSET, "important");
  statuses.add(ProjectState.BUILDING, "important");
  statuses.add(ProjectState.MERGING_LOGS, "important");
  statuses.add(ProjectState.QUEUED, "normal");
  statuses.add(ProjectState.WAITING, "dull");
  statuses.add(ProjectState.IDLE, "dull");
  statuses.add(ProjectState.PAUSED, "dull");
  statuses.add(ProjectState.STOPPED, "dull");
%>
<cruisecontrol:jmxbase id="jmxBase"/>
<%
  String name = System.getProperty("ccname", "");
  String hostname = InetAddress.getLocalHost().getHostName();
  boolean jmxEnabled = true;
  URL jmxURLPrefix = new URL(jmxBase, "invoke?operation=build&objectname=CruiseControl+Project%3Aname%3D");

  final String statusFileName = application.getInitParameter("currentBuildStatusFile");

  String baseURL = request.getScheme() + "://" + request.getServerName() + ":" + request.getServerPort()
                   + request.getContextPath() + "/";
  String thisURL = request.getRequestURI();

  String sort = request.getParameter("sort");
  if(sort == null){
    sort = "none";
  }
%>


<%
class TestInfo {
	private boolean pass;
	private String totalTest;
	private String totalFailed;
    private String project;
    private String desc;
    private int i;
    private int totalTestInt;
    private int totalFailedInt;
	
	public TestInfo(String project) throws ParseException, IOException {
      this.project = project;
		desc = "Build Failed!";
		totalFailed = "0";
		totalFailedInt = 0;
		totalTest = "0";
		totalTestInt = 0;
		pass = false;
		java.io.File prjDir = new java.io.File(System.getProperty("user.dir")+"\\projects\\"+project+"\\target\\test-results");
		String[] files = prjDir.list(new java.io.FilenameFilter() {
			public boolean accept(File dir, String filename) {
			boolean fileOK = true;
			fileOK &= filename.endsWith(".xml");
//			fileOK &= filename.startsWith("ts_");
			return fileOK;
			}
		});
		if (files != null) {
			if (files.length != 0) 
			{
				desc = "";
				for (int j = 0; j < files.length; j++) {
					desc += files[j];
					
					File trFile = new File(System.getProperty("user.dir")+"\\projects\\"+project+"\\target\\test-results",files[j]);
					BufferedReader reader = null;
					try {
						i = 0;
						String temp=null;
						reader = new BufferedReader(new FileReader(trFile));
						desc = reader.readLine();
						if (reader.readLine().compareTo("<testsuites>") == 0) {
							// get last line of report (summary)
							while ((desc = reader.readLine()) != null)
							{
							   temp=desc;
							}
							desc=temp;
							//desc = reader.readLine().replaceAll("<testsuite name=", "");
							i = desc.indexOf("failures=");
							totalFailed = desc.substring(i+10,i+17);
							i = desc.indexOf("tests=");
							totalTest = desc.substring(i+7,i+14);
							totalFailed = totalFailed.substring(0,totalFailed.indexOf("\""));
							totalTest = totalTest.substring(0,totalTest.indexOf("\""));
							totalTestInt += Integer.parseInt(totalTest);
							totalFailedInt += Integer.parseInt(totalFailed);
							desc ="";
//							if (totalFailed.compareTo("0") != 0) {
							if (totalFailedInt != 0) {
								pass = false;
							}
							else {
								pass = true;
							}
						}
					}
					catch (Exception e) {
						desc = "Build Failed!";
						totalFailed = "0";
						totalTest = "0";
						pass = false;
					}
					finally {
						if (reader != null) {
							reader.close();
						}
					}
				}
					

			}
		}
	}

    public String getDesc() {
      return totalTest+"/"+totalFailed;
    }
    public String getTotal() {
      return Integer.toString(totalTestInt);
    }
    public String getFailed() {
//      return desc;
      return Integer.toString(totalFailedInt);
//      return totalFailed;
    }
    public boolean getPass() {
      return pass;
    }
	
}


  class Info implements Comparable {
    public static final int ONE_DAY = 1000 * 60 * 60 * 24;

    private BuildInfo latest;
    private BuildInfo lastSuccessful;
    private SortableStatus status;
    private Date statusSince;
    private String project;
    private String statusDescription;

    public Info(File logsDir, String project) throws ParseException, IOException {
      this.project = project;

      File projectLogDir = new File(logsDir, project);
      LogFile latestLogFile = LogFile.getLatestLogFile(projectLogDir);
      LogFile latestSuccessfulLogFile = LogFile.getLatestSuccessfulLogFile(projectLogDir);


      if (latestLogFile != null) {
        latest = new BuildInfo(latestLogFile);
      }
      if (latestSuccessfulLogFile != null) {
        lastSuccessful = new BuildInfo(latestSuccessfulLogFile);
      }

      File statusFile = new File(projectLogDir, statusFileName);
      BufferedReader reader = null;
      try {
        reader = new BufferedReader(new FileReader(statusFile));
        statusDescription = reader.readLine().replaceAll(" since", "");

        status = statuses.get(statusDescription);
        statusSince = new Date(statusFile.lastModified());
      }
      catch (Exception e) {
        status = statuses.unknown;
        statusSince = now;
      }
      finally {
        if (reader != null) {
          reader.close();
        }
      }
    }

    public String getLastBuildTime() {
      return getTime(latest);
    }

    public String getLastSuccessfulBuildTime() {
      return getTime(lastSuccessful);
    }

    private String getTime(BuildInfo build) {
      return build != null ? format(build.getBuildDate()) : "";
    }

    public String format(Date date) {
      if (date == null) {
        return "";
      }

      if ((now.getTime() < date.getTime())) {
        return dateTimeFormat.format(date);
      }

      if ((now.getTime() - date.getTime()) < ONE_DAY) {
        return timeOnlyFormat.format(date);
      }

      return dateOnlyFormat.format(date);
    }

    public String getStatusSince() {
      return statusSince != null ? format(statusSince) : "?";
    }

    public boolean failed() {
      return latest == null || ! latest.isSuccessful();
    }

    public SortableStatus getStatus() {
      return status;
    }

    public int compareTo(Object other) {
      Info that = (Info) other;

      int order = this.status.compareTo(that.status);
      if (order != 0) {
        return order;
      }

      return (int) (this.statusSince.getTime() - that.statusSince.getTime());
    }

    public String getLabel() {
      return lastSuccessful != null ? lastSuccessful.getLabel() : " ";
    }
  }

%>

<%
  final Map sortOrders = new HashMap();

  sortOrders.put("project", new Comparator() {
    public int compare(Object a, Object b) {
      Info infoA = (Info) a;
      Info infoB = (Info) b;
      return infoA.project.compareTo(infoB.project);
    }
  });

  sortOrders.put("status", new Comparator() {
    public int compare(Object a, Object b) {
      Info infoA = (Info) a;
      Info infoB = (Info) b;
      return infoA.compareTo(infoB);
    }
  });

  sortOrders.put("label", new Comparator() {
    public int compare(Object a, Object b) {
      Info infoA = (Info) a;
      Info infoB = (Info) b;
      return infoA.getLabel().compareTo(infoB.getLabel());
    }
  });

  sortOrders.put("last failure", new Comparator() {
    public int compare(Object a, Object b) {
      Info infoA = (Info) a;
      Info infoB = (Info) b;

      if (infoA.latest == null) {
        return 1;
      }

      if (infoB.latest == null) {
        return -1;
      }

      if (infoA.failed() != infoB.failed()) {
        return infoA.failed() ? -1 : 1;
      }

      return infoB.latest.compareTo(infoA.latest);
    }
  });

  sortOrders.put("last successful", new Comparator() {
    public int compare(Object a, Object b) {
      Info infoA = (Info) a;
      Info infoB = (Info) b;

      if (infoA.lastSuccessful == null) {
        return 1;
      }
      if (infoB.lastSuccessful == null) {
        return -1;
      }

      return infoB.lastSuccessful.compareTo(infoA.lastSuccessful);
    }
  });
%>

<html>
<head>
  <title><%= name%> CruiseControl at ZCZ01-1107 </title>
<!-- <%= hostname %> -->
  <base href="<%=baseURL%>"/>
  <link type="application/rss+xml" rel="alternate" href="rss" title="RSS"/>
  <link type="text/css" rel="stylesheet" href="css/cruisecontrol.css"/>
  <META HTTP-EQUIV="Refresh" CONTENT="60" URL="<%=thisURL%>?sort=<%=sort%>">

  <style type="text/css">
    thead td {
      padding: 2 5
    }

    .data {
      padding: 2 5
    }

    .date {
      text-align: right;
    }

    .status-important {
      font-weight: bold;
    }

    .status-normal {
    }

    .status-dull {
      font-style: italic;
    }

    .failure {
      color: red;
      font-weight: bold
    }

    .currently-failing {
      color: red;
      font-weight: bold
    }

    .currently-passing {
      color: gray;
    }

    a.sort {
      color: firebrick;
    }

    a.sorted {
      color: darkblue;
    }

    .dateNow {
      font-size: smaller;
      font-style: italic;
    }

    .odd-row {
      background-color: #CCCCCC;
    }

    /*.even-row {
      background-color: #FFFFCC;
    }*/

	.building-row {
      background-color: #FF9900;
    }
	
    .header-row {
      background-color: white;
      color: darkblue;
    }
</style>

  <script language="JavaScript">
    function callServer(url, projectName) {
      document.getElementById('serverData').innerHTML = '<iframe src="' + url + '" width="0" height="0" frameborder="0"></iframe>';
      alert('Scheduling build for ' + projectName);
    }

    function checkIframe(stylesheetURL) {
      if (top != self) {//We are being framed!

        //For Internet Explorer
        if (document.createStyleSheet) {
          document.createStyleSheet(stylesheetURL);

        }
        else { //Non-ie browsers

          var styles = "@import url('" + stylesheetURL + "');";

          var newSS = document.createElement('link');

          newSS.rel = 'stylesheet';

          newSS.href = 'data:text/css,' + escape(styles);

          document.getElementsByTagName("head")[0].appendChild(newSS);

        }
      }
    }
  </script>
</head>


<body background="images/bluebg.gif" topmargin="0" leftmargin="0" marginheight="0" marginwidth="0"
      onload="checkIframe('<%=baseURL + "css/cruisecontrol.css"%>')">
<p>&nbsp;</p>

<h1 class="white" align="center"><%= name%><P>CruiseControl at ZCZ01-1107 - 
           <span class="dateNow">[<%= dateNow %>]</span></P></h1>

<!-- <%= hostname %> -->
<div id="serverData" class="hidden"></div>

<h2 class="white" align="center">Go to ...<a href="test_reports">Test Reports</a></h2>
<h2 class="white" align="center">Go to ...<a href="http://vav.freescale.net/aas/">Artifacts Archive</a></h2>
<h2 class="white" align="center">Go to ...<a href="http://ZCZ01-1107/">Control Pannel</a></h2>


<div class="fstitle" align="center">
<h3 class="fstitle" align="center">MQX - Sanity check</h3>

</div>

<% String logDirPath;  %>
<form>

<!----------------------------------------------------------------------------->
<!------------------------- INTERNAL TESTS  ----------------------------------->
<!----------------------------------------------------------------------------->
<h2 class="white" align="center">Internal tests</h2>
<table align="center" border="0" cellpadding="0" cellspacing="0" >
<tr>
<!----------------------------------------------------------------------------->
<!------------------------- FIRST COMPILER ----------------------------------->
<!----------------------------------------------------------------------------->
<%@ include file="jsp_includes/compiler_table_IAR.jsp" %>
</tr>
<tr>
<!----------------------------------------------------------------------------->
<!------------------------- SECOND COMPILER ----------------------------------->
<!----------------------------------------------------------------------------->
<%@ include file="jsp_includes/compiler_table_CWMCU63.jsp" %>
</tr>
<tr>
<!----------------------------------------------------------------------------->
<!------------------------- THIRD COMPILER ----------------------------------->
<!----------------------------------------------------------------------------->
<%@ include file="jsp_includes/compiler_table_CWCF72.jsp" %>
</tr>
<tr>
<!----------------------------------------------------------------------------->
<!------------------------- FOURTH COMPILER ----------------------------------->
<!----------------------------------------------------------------------------->
<%@ include file="jsp_includes/compiler_table_CW10.jsp" %>
</tr>
<tr>
<!----------------------------------------------------------------------------->
<!------------------------- FIFTH COMPILER ------------------------------------>
<!----------------------------------------------------------------------------->
<%@ include file="jsp_includes/compiler_table_CW88.jsp" %>
</tr>
</table>

<h2 class="white" align="center">Platform keeper</h2>
<table align="center" border="0" cellpadding="0" cellspacing="0" >
<tr>
<!----------------------------------------------------------------------------->
<!------------------------- PLATFORM KEEPER ----------------------------------->
<!----------------------------------------------------------------------------->
<%@ include file="jsp_includes/compiler_table_KEEPER.jsp" %>
</tr>
</table>

    <tfoot>
        <tr>
            <td colspan="2" align="right"><a href="rss"><img border="0" src="images/rss.png"/></a></td>
        </tr>
    </tfoot>
  </table>
</form>
</body>
</html>

