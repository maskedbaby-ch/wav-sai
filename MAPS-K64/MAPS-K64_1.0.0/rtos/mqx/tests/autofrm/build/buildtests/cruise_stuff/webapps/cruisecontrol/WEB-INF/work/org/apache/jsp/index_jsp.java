package org.apache.jsp;

import javax.servlet.*;
import javax.servlet.http.*;
import javax.servlet.jsp.*;
import net.sourceforge.cruisecontrol.*;
import java.io.IOException;
import java.net.InetAddress;
import java.net.URL;
import java.text.DateFormat;
import java.io.File;
import java.util.Arrays;
import java.text.ParseException;
import java.io.BufferedReader;
import java.io.FileReader;
import java.util.HashMap;
import java.util.Date;
import java.util.Comparator;
import java.util.Map;

public final class index_jsp extends org.apache.jasper.runtime.HttpJspBase
    implements org.apache.jasper.runtime.JspSourceDependent {

  private static java.util.Vector _jspx_dependants;

  static {
    _jspx_dependants = new java.util.Vector(6);
    _jspx_dependants.add("/WEB-INF/cruisecontrol-jsp11.tld");
    _jspx_dependants.add("/jsp_includes/compiler_table_IAR.jsp");
    _jspx_dependants.add("/jsp_includes/compiler_table_CWMCU63.jsp");
    _jspx_dependants.add("/jsp_includes/compiler_table_CWCF72.jsp");
    _jspx_dependants.add("/jsp_includes/compiler_table_CW10.jsp");
    _jspx_dependants.add("/jsp_includes/compiler_table_CW88.jsp");
    _jspx_dependants.add("/jsp_includes/compiler_table_KEEPER.jsp");
  }

  private org.apache.jasper.runtime.TagHandlerPool _jspx_tagPool_cruisecontrol_jmxbase_id_nobody;

  public java.util.List getDependants() {
    return _jspx_dependants;
  }

  public void _jspInit() {
    _jspx_tagPool_cruisecontrol_jmxbase_id_nobody = org.apache.jasper.runtime.TagHandlerPool.getTagHandlerPool(getServletConfig());
  }

  public void _jspDestroy() {
    _jspx_tagPool_cruisecontrol_jmxbase_id_nobody.release();
  }

  public void _jspService(HttpServletRequest request, HttpServletResponse response)
        throws java.io.IOException, ServletException {

    JspFactory _jspxFactory = null;
    PageContext pageContext = null;
    HttpSession session = null;
    ServletContext application = null;
    ServletConfig config = null;
    JspWriter out = null;
    Object page = this;
    JspWriter _jspx_out = null;
    PageContext _jspx_page_context = null;


    try {
      _jspxFactory = JspFactory.getDefaultFactory();
      response.setContentType("text/html; charset=UTF-8");
      pageContext = _jspxFactory.getPageContext(this, request, response,
      			"/error.jsp", true, 8192, true);
      _jspx_page_context = pageContext;
      application = pageContext.getServletContext();
      config = pageContext.getServletConfig();
      session = pageContext.getSession();
      out = pageContext.getOut();
      _jspx_out = out;

      out.write("\r\n");
      out.write("\r\n");
      out.write("\r\n");
      out.write("\r\n");
      out.write("\r\n");
      out.write("\r\n");
      out.write("\r\n");
      out.write("\r\n");
      out.write("\r\n");
      out.write("\r\n");
      out.write("\r\n");
      out.write("\r\n");
      out.write("\r\n");
      out.write("\r\n");
      out.write("\r\n");
      out.write("\r\n");
      out.write("\r\n");
      out.write("\r\n");
      out.write("\r\n");

  final DateFormat dateTimeFormat = DateFormat.getDateTimeInstance(DateFormat.SHORT, DateFormat.SHORT, request.getLocale());
  final DateFormat dateOnlyFormat = DateFormat.getDateInstance(DateFormat.SHORT, request.getLocale());
  final DateFormat timeOnlyFormat = DateFormat.getTimeInstance(DateFormat.SHORT, request.getLocale());

  final Date now = new Date();
  final String dateNow = dateTimeFormat.format(now);

      out.write("\r\n");
      out.write("\r\n");



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


      out.write("\r\n");
      out.write("\r\n");

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

      out.write('\r');
      out.write('\n');
      //  cruisecontrol:jmxbase
      java.net.URL jmxBase = null;
      net.sourceforge.cruisecontrol.taglib.JmxBaseTag _jspx_th_cruisecontrol_jmxbase_0 = (net.sourceforge.cruisecontrol.taglib.JmxBaseTag) _jspx_tagPool_cruisecontrol_jmxbase_id_nobody.get(net.sourceforge.cruisecontrol.taglib.JmxBaseTag.class);
      _jspx_th_cruisecontrol_jmxbase_0.setPageContext(_jspx_page_context);
      _jspx_th_cruisecontrol_jmxbase_0.setParent(null);
      _jspx_th_cruisecontrol_jmxbase_0.setId("jmxBase");
      int _jspx_eval_cruisecontrol_jmxbase_0 = _jspx_th_cruisecontrol_jmxbase_0.doStartTag();
      jmxBase = (java.net.URL) _jspx_page_context.findAttribute("jmxBase");
      if (_jspx_th_cruisecontrol_jmxbase_0.doEndTag() == javax.servlet.jsp.tagext.Tag.SKIP_PAGE)
        return;
      jmxBase = (java.net.URL) _jspx_page_context.findAttribute("jmxBase");
      _jspx_tagPool_cruisecontrol_jmxbase_id_nobody.reuse(_jspx_th_cruisecontrol_jmxbase_0);
      out.write('\r');
      out.write('\n');

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

      out.write("\r\n");
      out.write("\r\n");
      out.write("\r\n");

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


      out.write("\r\n");
      out.write("\r\n");

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

      out.write("\r\n");
      out.write("\r\n");
      out.write("<html>\r\n");
      out.write("<head>\r\n");
      out.write("  <title>");
      out.print( name);
      out.write(" CruiseControl at ZCZ01-1107 </title>\r\n");
      out.write("<!-- ");
      out.print( hostname );
      out.write(" -->\r\n");
      out.write("  <base href=\"");
      out.print(baseURL);
      out.write("\"/>\r\n");
      out.write("  <link type=\"application/rss+xml\" rel=\"alternate\" href=\"rss\" title=\"RSS\"/>\r\n");
      out.write("  <link type=\"text/css\" rel=\"stylesheet\" href=\"css/cruisecontrol.css\"/>\r\n");
      out.write("  <META HTTP-EQUIV=\"Refresh\" CONTENT=\"60\" URL=\"");
      out.print(thisURL);
      out.write("?sort=");
      out.print(sort);
      out.write("\">\r\n");
      out.write("\r\n");
      out.write("  <style type=\"text/css\">\r\n");
      out.write("    thead td {\r\n");
      out.write("      padding: 2 5\r\n");
      out.write("    }\r\n");
      out.write("\r\n");
      out.write("    .data {\r\n");
      out.write("      padding: 2 5\r\n");
      out.write("    }\r\n");
      out.write("\r\n");
      out.write("    .date {\r\n");
      out.write("      text-align: right;\r\n");
      out.write("    }\r\n");
      out.write("\r\n");
      out.write("    .status-important {\r\n");
      out.write("      font-weight: bold;\r\n");
      out.write("    }\r\n");
      out.write("\r\n");
      out.write("    .status-normal {\r\n");
      out.write("    }\r\n");
      out.write("\r\n");
      out.write("    .status-dull {\r\n");
      out.write("      font-style: italic;\r\n");
      out.write("    }\r\n");
      out.write("\r\n");
      out.write("    .failure {\r\n");
      out.write("      color: red;\r\n");
      out.write("      font-weight: bold\r\n");
      out.write("    }\r\n");
      out.write("\r\n");
      out.write("    .currently-failing {\r\n");
      out.write("      color: red;\r\n");
      out.write("      font-weight: bold\r\n");
      out.write("    }\r\n");
      out.write("\r\n");
      out.write("    .currently-passing {\r\n");
      out.write("      color: gray;\r\n");
      out.write("    }\r\n");
      out.write("\r\n");
      out.write("    a.sort {\r\n");
      out.write("      color: firebrick;\r\n");
      out.write("    }\r\n");
      out.write("\r\n");
      out.write("    a.sorted {\r\n");
      out.write("      color: darkblue;\r\n");
      out.write("    }\r\n");
      out.write("\r\n");
      out.write("    .dateNow {\r\n");
      out.write("      font-size: smaller;\r\n");
      out.write("      font-style: italic;\r\n");
      out.write("    }\r\n");
      out.write("\r\n");
      out.write("    .odd-row {\r\n");
      out.write("      background-color: #CCCCCC;\r\n");
      out.write("    }\r\n");
      out.write("\r\n");
      out.write("    /*.even-row {\r\n");
      out.write("      background-color: #FFFFCC;\r\n");
      out.write("    }*/\r\n");
      out.write("\r\n");
      out.write("\t.building-row {\r\n");
      out.write("      background-color: #FF9900;\r\n");
      out.write("    }\r\n");
      out.write("\t\r\n");
      out.write("    .header-row {\r\n");
      out.write("      background-color: white;\r\n");
      out.write("      color: darkblue;\r\n");
      out.write("    }\r\n");
      out.write("</style>\r\n");
      out.write("\r\n");
      out.write("  <script language=\"JavaScript\">\r\n");
      out.write("    function callServer(url, projectName) {\r\n");
      out.write("      document.getElementById('serverData').innerHTML = '<iframe src=\"' + url + '\" width=\"0\" height=\"0\" frameborder=\"0\"></iframe>';\r\n");
      out.write("      alert('Scheduling build for ' + projectName);\r\n");
      out.write("    }\r\n");
      out.write("\r\n");
      out.write("    function checkIframe(stylesheetURL) {\r\n");
      out.write("      if (top != self) {//We are being framed!\r\n");
      out.write("\r\n");
      out.write("        //For Internet Explorer\r\n");
      out.write("        if (document.createStyleSheet) {\r\n");
      out.write("          document.createStyleSheet(stylesheetURL);\r\n");
      out.write("\r\n");
      out.write("        }\r\n");
      out.write("        else { //Non-ie browsers\r\n");
      out.write("\r\n");
      out.write("          var styles = \"@import url('\" + stylesheetURL + \"');\";\r\n");
      out.write("\r\n");
      out.write("          var newSS = document.createElement('link');\r\n");
      out.write("\r\n");
      out.write("          newSS.rel = 'stylesheet';\r\n");
      out.write("\r\n");
      out.write("          newSS.href = 'data:text/css,' + escape(styles);\r\n");
      out.write("\r\n");
      out.write("          document.getElementsByTagName(\"head\")[0].appendChild(newSS);\r\n");
      out.write("\r\n");
      out.write("        }\r\n");
      out.write("      }\r\n");
      out.write("    }\r\n");
      out.write("  </script>\r\n");
      out.write("</head>\r\n");
      out.write("\r\n");
      out.write("\r\n");
      out.write("<body background=\"images/bluebg.gif\" topmargin=\"0\" leftmargin=\"0\" marginheight=\"0\" marginwidth=\"0\"\r\n");
      out.write("      onload=\"checkIframe('");
      out.print(baseURL + "css/cruisecontrol.css");
      out.write("')\">\r\n");
      out.write("<p>&nbsp;</p>\r\n");
      out.write("\r\n");
      out.write("<h1 class=\"white\" align=\"center\">");
      out.print( name);
      out.write("<P>CruiseControl at ZCZ01-1107 - \r\n");
      out.write("           <span class=\"dateNow\">[");
      out.print( dateNow );
      out.write("]</span></P></h1>\r\n");
      out.write("\r\n");
      out.write("<!-- ");
      out.print( hostname );
      out.write(" -->\r\n");
      out.write("<div id=\"serverData\" class=\"hidden\"></div>\r\n");
      out.write("\r\n");
      out.write("<h2 class=\"white\" align=\"center\">Go to ...<a href=\"test_reports\">Test Reports</a></h2>\r\n");
      out.write("<h2 class=\"white\" align=\"center\">Go to ...<a href=\"http://vav.freescale.net/aas/\">Artifacts Archive</a></h2>\r\n");
      out.write("<h2 class=\"white\" align=\"center\">Go to ...<a href=\"http://ZCZ01-1107/\">Control Pannel</a></h2>\r\n");
      out.write("\r\n");
      out.write("\r\n");
      out.write("<div class=\"fstitle\" align=\"center\">\r\n");
      out.write("<h3 class=\"fstitle\" align=\"center\">MQX - Sanity check</h3>\r\n");
      out.write("\r\n");
      out.write("</div>\r\n");
      out.write("\r\n");
 String logDirPath;  
      out.write("\r\n");
      out.write("<form>\r\n");
      out.write("\r\n");
      out.write("<!----------------------------------------------------------------------------->\r\n");
      out.write("<!------------------------- INTERNAL TESTS  ----------------------------------->\r\n");
      out.write("<!----------------------------------------------------------------------------->\r\n");
      out.write("<h2 class=\"white\" align=\"center\">Internal tests</h2>\r\n");
      out.write("<table align=\"center\" border=\"0\" cellpadding=\"0\" cellspacing=\"0\" >\r\n");
      out.write("<tr>\r\n");
      out.write("<!----------------------------------------------------------------------------->\r\n");
      out.write("<!------------------------- FIRST COMPILER ----------------------------------->\r\n");
      out.write("<!----------------------------------------------------------------------------->\r\n");
      out.write("<!----------------------------------------------------------------------------->\r\n");
      out.write("<!-------------------------------  COMPILER iar ---------------------->\r\n");
      out.write("<!----------------------------------------------------------------------------->\r\n");
      out.write(" <td><h4 class=\"fsbolero\" align=\"center\" >iar</h4>\r\n");
      out.write("  <table align=\"center\" border=\"0\" cellpadding=\"0\" cellspacing=\"0\">\r\n");
      out.write("    <tbody>\r\n");
      out.write("      <tr><td colspan=\"2\">&nbsp;</td></tr>\r\n");
      out.write("      <tr>\r\n");
      out.write("        <td class=\"header-row\"><img border=\"0\" src=\"images/bluestripestop.gif\"/></td>\r\n");
      out.write("        <td class=\"header-row\" align=\"right\"><img border=\"0\" src=\"images/bluestripestopright.gif\"/></td>\r\n");
      out.write("      </tr>\r\n");
      out.write("      <tr><td colspan=\"2\">\r\n");
      out.write("        <table class=\"index\" width=\"100%\" cellpadding=\"0\" cellspacing=\"1\">\r\n");
      out.write("          ");

            logDirPath = application.getInitParameter("logDir");
            if (logDirPath == null) {
          
      out.write("<tr><td>You need to provide a value for the context parameter <code>&quot;logDir&quot;</code></td></tr>");

        }
        else {
          java.io.File logDir_iar = new java.io.File(logDirPath);
          if (logDir_iar.isDirectory() == false) {
        
      out.write("<tr><td>Context parameter logDir needs to be set to a directory. Currently set to &quot;");
      out.print(logDirPath);
      out.write("\r\n");
      out.write("          &quot;</td></tr>");

        }
        else {
          String[] projectDirs = logDir_iar.list(new java.io.FilenameFilter() {
            public boolean accept(File dir, String filename) {
               boolean fileOK = true;
				fileOK &= filename.endsWith("iar");
				fileOK &= filename.startsWith("ar_int_");
               return fileOK;
            }
          });
          if (projectDirs.length == 0) {
        
      out.write("<tr><td>no project directories found under ");
      out.print(logDirPath);
      out.write("</td></tr>");

        }
        else {
        
      out.write(" <thead class=\"index-header\">\r\n");
      out.write("          <tr class=\"header-row\">\r\n");
      out.write("\t\t\t<td  class=\"project\">Module</td>\r\n");
      out.write("\t\t\t<td  class=\"project\">Project name</td>\r\n");
      out.write("            <td  class=\"status\">Last build</td>\r\n");
      out.write("            <td  class=\"data\">Label</td>\r\n");
      out.write("            <td  class=\"project\">Tests/Failed</td>\r\n");
      out.write("        </tr>\r\n");
      out.write("        </thead>\r\n");
      out.write("          <tbody>\r\n");
      out.write("            ");

              Info[] info = new Info[projectDirs.length];
              for (int i = 0; i < info.length; i++) {
                info[i] = new Info(logDir_iar, projectDirs[i]);
              }
              Comparator order = (Comparator) sortOrders.get(sort);
              if (order == null) {
              }
              else {
                Arrays.sort(info, order);
              }

              for (int i = 0; i < info.length; i++) {
				 TestInfo testInfo = new TestInfo(info[i].project);
				 String color_green=(i % 2 == 1) ? "#22CC22" : "#11AA11";
				 String color_red=(i % 2 == 1) ? "#FF2222" : "#CC1111";
				 String module_name_color=(i % 2 == 1) ? "#FFFFAA" : "#CCCCAA";
				 String bgcolor_class=(i % 2 == 1) ? "even-row" : "odd-row";
				 if (info[i].getStatus().toString().equals("building") || info[i].getStatus().toString().equals("publishing")){
					bgcolor_class="building-row";
					color_green="";
					color_red="";
					module_name_color="";
				 }
				 
				 String moduleName=info[i].project.replaceAll("ar_int_","");
				 moduleName=moduleName.replaceAll("_iar","");
            
      out.write("\r\n");
      out.write("\t\t<tr class=\"");
      out.print( bgcolor_class );
      out.write(" \">\r\n");
      out.write("              <td  rowspan=\"2\" align=\"center\" bgcolor=\"");
      out.print( module_name_color );
      out.write(" \" ><b>");
      out.print(moduleName);
      out.write("</b></td>\r\n");
      out.write("\t\t\t  <td  class=\"data\"><a href=\"buildresults/");
      out.print(info[i].project);
      out.write('"');
      out.write('>');
      out.print(info[i].project);
      out.write("</a></td>\r\n");
      out.write("              <td  class=\"");
      out.print( (info[i].failed()) ? "data date failure" : "data date" );
      out.write('"');
      out.write('>');
      out.print( (info[i].failed()) ? info[i].getLastBuildTime() : info[i].getLastSuccessfulBuildTime() );
      out.write("</td>\r\n");
      out.write("              <td  class=\"data\">");
      out.print( info[i].getLabel());
      out.write("</td>\r\n");
      out.write("              <td  rowspan=\"2\" align=\"center\" bgcolor=");
      out.print( (testInfo.getPass()) ? color_green : color_red );
      out.write('>');
      out.print( testInfo.getTotal() );
      out.write(' ');
      out.write('/');
      out.write(' ');
      out.print( testInfo.getFailed() );
      out.write("\r\n");
      out.write("\t\t<tr class=\"");
      out.print( bgcolor_class );
      out.write(" \">\r\n");
      out.write("              <td align=\"center\" colspan=2 class=\"data date status-");
      out.print( info[i].getStatus().getImportance() );
      out.write("\"><small>");
      out.print( info[i].getStatus());
      out.write(" <em>(");
      out.print( info[i].getStatusSince() );
      out.write(")</em></small></td>\r\n");
      out.write("              ");
 if (jmxEnabled) { 
      out.write("\r\n");
      out.write("              <td class=\"data\"><!-- <input id=\"");
      out.print( "force_" + info[i].project );
      out.write("\" type=\"button\"\r\n");
      out.write("                                      onclick=\"callServer('");
      out.print( jmxURLPrefix.toExternalForm() + info[i].project );
      out.write("', '");
      out.print(info[i].project);
      out.write("')\"\r\n");
      out.write("                                      value=\"Build\"/> --></td>\r\n");
      out.write("              ");
 } 
      out.write("\r\n");
      out.write("            </tr>\r\n");
      out.write("          </tbody>\r\n");
      out.write("          ");

                  }
                }
              }
            }
          
      out.write("</table>\r\n");
      out.write("      </td></tr>\r\n");
      out.write("      <tr>\r\n");
      out.write("        <td bgcolor=\"#FFFFFF\"><img border=\"0\" src=\"images/bluestripesbottom.gif\"/></td>\r\n");
      out.write("        <td align=\"right\" bgcolor=\"#FFFFFF\"><img border=\"0\" src=\"images/bluestripesbottomright.gif\"/></td>\r\n");
      out.write("      </tr>\r\n");
      out.write("      <tr><td colspan=\"2\">&nbsp;</td></tr>\r\n");
      out.write("    </tbody>\r\n");
      out.write("  </table>\r\n");
      out.write("  <td>&nbsp;</td>\r\n");
      out.write("\r\n");
      out.write("</tr>\r\n");
      out.write("<tr>\r\n");
      out.write("<!----------------------------------------------------------------------------->\r\n");
      out.write("<!------------------------- SECOND COMPILER ----------------------------------->\r\n");
      out.write("<!----------------------------------------------------------------------------->\r\n");
      out.write("<!----------------------------------------------------------------------------->\r\n");
      out.write("<!-------------------------------  COMPILER cwmcu63 ---------------------->\r\n");
      out.write("<!----------------------------------------------------------------------------->\r\n");
      out.write(" <td><h4 class=\"fsbolero\" align=\"center\" >cwmcu63</h4>\r\n");
      out.write("  <table align=\"center\" border=\"0\" cellpadding=\"0\" cellspacing=\"0\">\r\n");
      out.write("    <tbody>\r\n");
      out.write("      <tr><td colspan=\"2\">&nbsp;</td></tr>\r\n");
      out.write("      <tr>\r\n");
      out.write("        <td class=\"header-row\"><img border=\"0\" src=\"images/bluestripestop.gif\"/></td>\r\n");
      out.write("        <td class=\"header-row\" align=\"right\"><img border=\"0\" src=\"images/bluestripestopright.gif\"/></td>\r\n");
      out.write("      </tr>\r\n");
      out.write("      <tr><td colspan=\"2\">\r\n");
      out.write("        <table class=\"index\" width=\"100%\" cellpadding=\"0\" cellspacing=\"1\">\r\n");
      out.write("          ");

            logDirPath = application.getInitParameter("logDir");
            if (logDirPath == null) {
          
      out.write("<tr><td>You need to provide a value for the context parameter <code>&quot;logDir&quot;</code></td></tr>");

        }
        else {
          java.io.File logDir_cwmcu63 = new java.io.File(logDirPath);
          if (logDir_cwmcu63.isDirectory() == false) {
        
      out.write("<tr><td>Context parameter logDir needs to be set to a directory. Currently set to &quot;");
      out.print(logDirPath);
      out.write("\r\n");
      out.write("          &quot;</td></tr>");

        }
        else {
          String[] projectDirs = logDir_cwmcu63.list(new java.io.FilenameFilter() {
            public boolean accept(File dir, String filename) {
               boolean fileOK = true;
				fileOK &= filename.endsWith("cwmcu63");
				fileOK &= filename.startsWith("ar_int_");
               return fileOK;
            }
          });
          if (projectDirs.length == 0) {
        
      out.write("<tr><td>no project directories found under ");
      out.print(logDirPath);
      out.write("</td></tr>");

        }
        else {
        
      out.write(" <thead class=\"index-header\">\r\n");
      out.write("          <tr class=\"header-row\">\r\n");
      out.write("\t\t\t<td  class=\"project\">Module</td>\r\n");
      out.write("\t\t\t<td  class=\"project\">Project name</td>\r\n");
      out.write("            <td  class=\"status\">Last build</td>\r\n");
      out.write("            <td  class=\"data\">Label</td>\r\n");
      out.write("            <td  class=\"project\">Tests/Failed</td>\r\n");
      out.write("        </tr>\r\n");
      out.write("        </thead>\r\n");
      out.write("          <tbody>\r\n");
      out.write("            ");

              Info[] info = new Info[projectDirs.length];
              for (int i = 0; i < info.length; i++) {
                info[i] = new Info(logDir_cwmcu63, projectDirs[i]);
              }
              Comparator order = (Comparator) sortOrders.get(sort);
              if (order == null) {
              }
              else {
                Arrays.sort(info, order);
              }

              for (int i = 0; i < info.length; i++) {
				 TestInfo testInfo = new TestInfo(info[i].project);
				 String color_green=(i % 2 == 1) ? "#22CC22" : "#11AA11";
				 String color_red=(i % 2 == 1) ? "#FF2222" : "#CC1111";
				 String module_name_color=(i % 2 == 1) ? "#FFFFAA" : "#CCCCAA";
				 String bgcolor_class=(i % 2 == 1) ? "even-row" : "odd-row";
				 if (info[i].getStatus().toString().equals("building") || info[i].getStatus().toString().equals("publishing")){
					bgcolor_class="building-row";
					color_green="";
					color_red="";
					module_name_color="";
				 }
				 
				 String moduleName=info[i].project.replaceAll("ar_int_","");
				 moduleName=moduleName.replaceAll("_cwmcu63","");
            
      out.write("\r\n");
      out.write("\t\t<tr class=\"");
      out.print( bgcolor_class );
      out.write(" \">\r\n");
      out.write("              <td  rowspan=\"2\" align=\"center\" bgcolor=\"");
      out.print( module_name_color );
      out.write(" \" ><b>");
      out.print(moduleName);
      out.write("</b></td>\r\n");
      out.write("\t\t\t  <td  class=\"data\"><a href=\"buildresults/");
      out.print(info[i].project);
      out.write('"');
      out.write('>');
      out.print(info[i].project);
      out.write("</a></td>\r\n");
      out.write("              <td  class=\"");
      out.print( (info[i].failed()) ? "data date failure" : "data date" );
      out.write('"');
      out.write('>');
      out.print( (info[i].failed()) ? info[i].getLastBuildTime() : info[i].getLastSuccessfulBuildTime() );
      out.write("</td>\r\n");
      out.write("              <td  class=\"data\">");
      out.print( info[i].getLabel());
      out.write("</td>\r\n");
      out.write("              <td  rowspan=\"2\" align=\"center\" bgcolor=");
      out.print( (testInfo.getPass()) ? color_green : color_red );
      out.write('>');
      out.print( testInfo.getTotal() );
      out.write(' ');
      out.write('/');
      out.write(' ');
      out.print( testInfo.getFailed() );
      out.write("\r\n");
      out.write("\t\t<tr class=\"");
      out.print( bgcolor_class );
      out.write(" \">\r\n");
      out.write("              <td align=\"center\" colspan=2 class=\"data date status-");
      out.print( info[i].getStatus().getImportance() );
      out.write("\"><small>");
      out.print( info[i].getStatus());
      out.write(" <em>(");
      out.print( info[i].getStatusSince() );
      out.write(")</em></small></td>\r\n");
      out.write("              ");
 if (jmxEnabled) { 
      out.write("\r\n");
      out.write("              <td class=\"data\"><!-- <input id=\"");
      out.print( "force_" + info[i].project );
      out.write("\" type=\"button\"\r\n");
      out.write("                                      onclick=\"callServer('");
      out.print( jmxURLPrefix.toExternalForm() + info[i].project );
      out.write("', '");
      out.print(info[i].project);
      out.write("')\"\r\n");
      out.write("                                      value=\"Build\"/> --></td>\r\n");
      out.write("              ");
 } 
      out.write("\r\n");
      out.write("            </tr>\r\n");
      out.write("          </tbody>\r\n");
      out.write("          ");

                  }
                }
              }
            }
          
      out.write("</table>\r\n");
      out.write("      </td></tr>\r\n");
      out.write("      <tr>\r\n");
      out.write("        <td bgcolor=\"#FFFFFF\"><img border=\"0\" src=\"images/bluestripesbottom.gif\"/></td>\r\n");
      out.write("        <td align=\"right\" bgcolor=\"#FFFFFF\"><img border=\"0\" src=\"images/bluestripesbottomright.gif\"/></td>\r\n");
      out.write("      </tr>\r\n");
      out.write("      <tr><td colspan=\"2\">&nbsp;</td></tr>\r\n");
      out.write("    </tbody>\r\n");
      out.write("  </table>\r\n");
      out.write("  <td>&nbsp;</td>\r\n");
      out.write("\r\n");
      out.write("</tr>\r\n");
      out.write("<tr>\r\n");
      out.write("<!----------------------------------------------------------------------------->\r\n");
      out.write("<!------------------------- THIRD COMPILER ----------------------------------->\r\n");
      out.write("<!----------------------------------------------------------------------------->\r\n");
      out.write("<!----------------------------------------------------------------------------->\r\n");
      out.write("<!-------------------------------  COMPILER cwcf72 ---------------------->\r\n");
      out.write("<!----------------------------------------------------------------------------->\r\n");
      out.write(" <td><h4 class=\"fsbolero\" align=\"center\" >cwcf72</h4>\r\n");
      out.write("  <table align=\"center\" border=\"0\" cellpadding=\"0\" cellspacing=\"0\">\r\n");
      out.write("    <tbody>\r\n");
      out.write("      <tr><td colspan=\"2\">&nbsp;</td></tr>\r\n");
      out.write("      <tr>\r\n");
      out.write("        <td class=\"header-row\"><img border=\"0\" src=\"images/bluestripestop.gif\"/></td>\r\n");
      out.write("        <td class=\"header-row\" align=\"right\"><img border=\"0\" src=\"images/bluestripestopright.gif\"/></td>\r\n");
      out.write("      </tr>\r\n");
      out.write("      <tr><td colspan=\"2\">\r\n");
      out.write("        <table class=\"index\" width=\"100%\" cellpadding=\"0\" cellspacing=\"1\">\r\n");
      out.write("          ");

            logDirPath = application.getInitParameter("logDir");
            if (logDirPath == null) {
          
      out.write("<tr><td>You need to provide a value for the context parameter <code>&quot;logDir&quot;</code></td></tr>");

        }
        else {
          java.io.File logDir_cwcf72 = new java.io.File(logDirPath);
          if (logDir_cwcf72.isDirectory() == false) {
        
      out.write("<tr><td>Context parameter logDir needs to be set to a directory. Currently set to &quot;");
      out.print(logDirPath);
      out.write("\r\n");
      out.write("          &quot;</td></tr>");

        }
        else {
          String[] projectDirs = logDir_cwcf72.list(new java.io.FilenameFilter() {
            public boolean accept(File dir, String filename) {
               boolean fileOK = true;
				fileOK &= filename.endsWith("cwcf72");
				fileOK &= filename.startsWith("ar_int_");
               return fileOK;
            }
          });
          if (projectDirs.length == 0) {
        
      out.write("<tr><td>no project directories found under ");
      out.print(logDirPath);
      out.write("</td></tr>");

        }
        else {
        
      out.write(" <thead class=\"index-header\">\r\n");
      out.write("          <tr class=\"header-row\">\r\n");
      out.write("\t\t\t<td  class=\"project\">Module</td>\r\n");
      out.write("\t\t\t<td  class=\"project\">Project name</td>\r\n");
      out.write("            <td  class=\"status\">Last build</td>\r\n");
      out.write("            <td  class=\"data\">Label</td>\r\n");
      out.write("            <td  class=\"project\">Tests/Failed</td>\r\n");
      out.write("        </tr>\r\n");
      out.write("        </thead>\r\n");
      out.write("          <tbody>\r\n");
      out.write("            ");

              Info[] info = new Info[projectDirs.length];
              for (int i = 0; i < info.length; i++) {
                info[i] = new Info(logDir_cwcf72, projectDirs[i]);
              }
              Comparator order = (Comparator) sortOrders.get(sort);
              if (order == null) {
              }
              else {
                Arrays.sort(info, order);
              }

              for (int i = 0; i < info.length; i++) {
				 TestInfo testInfo = new TestInfo(info[i].project);
				 String color_green=(i % 2 == 1) ? "#22CC22" : "#11AA11";
				 String color_red=(i % 2 == 1) ? "#FF2222" : "#CC1111";
				 String module_name_color=(i % 2 == 1) ? "#FFFFAA" : "#CCCCAA";
				 String bgcolor_class=(i % 2 == 1) ? "even-row" : "odd-row";
				 if (info[i].getStatus().toString().equals("building") || info[i].getStatus().toString().equals("publishing")){
					bgcolor_class="building-row";
					color_green="";
					color_red="";
					module_name_color="";
				 }
				 
				 String moduleName=info[i].project.replaceAll("ar_int_","");
				 moduleName=moduleName.replaceAll("_cwcf72","");
            
      out.write("\r\n");
      out.write("\t\t<tr class=\"");
      out.print( bgcolor_class );
      out.write(" \">\r\n");
      out.write("              <td  rowspan=\"2\" align=\"center\" bgcolor=\"");
      out.print( module_name_color );
      out.write(" \" ><b>");
      out.print(moduleName);
      out.write("</b></td>\r\n");
      out.write("\t\t\t  <td  class=\"data\"><a href=\"buildresults/");
      out.print(info[i].project);
      out.write('"');
      out.write('>');
      out.print(info[i].project);
      out.write("</a></td>\r\n");
      out.write("              <td  class=\"");
      out.print( (info[i].failed()) ? "data date failure" : "data date" );
      out.write('"');
      out.write('>');
      out.print( (info[i].failed()) ? info[i].getLastBuildTime() : info[i].getLastSuccessfulBuildTime() );
      out.write("</td>\r\n");
      out.write("              <td  class=\"data\">");
      out.print( info[i].getLabel());
      out.write("</td>\r\n");
      out.write("              <td  rowspan=\"2\" align=\"center\" bgcolor=");
      out.print( (testInfo.getPass()) ? color_green : color_red );
      out.write('>');
      out.print( testInfo.getTotal() );
      out.write(' ');
      out.write('/');
      out.write(' ');
      out.print( testInfo.getFailed() );
      out.write("\r\n");
      out.write("\t\t<tr class=\"");
      out.print( bgcolor_class );
      out.write(" \">\r\n");
      out.write("              <td align=\"center\" colspan=2 class=\"data date status-");
      out.print( info[i].getStatus().getImportance() );
      out.write("\"><small>");
      out.print( info[i].getStatus());
      out.write(" <em>(");
      out.print( info[i].getStatusSince() );
      out.write(")</em></small></td>\r\n");
      out.write("              ");
 if (jmxEnabled) { 
      out.write("\r\n");
      out.write("              <td class=\"data\"><!-- <input id=\"");
      out.print( "force_" + info[i].project );
      out.write("\" type=\"button\"\r\n");
      out.write("                                      onclick=\"callServer('");
      out.print( jmxURLPrefix.toExternalForm() + info[i].project );
      out.write("', '");
      out.print(info[i].project);
      out.write("')\"\r\n");
      out.write("                                      value=\"Build\"/> --></td>\r\n");
      out.write("              ");
 } 
      out.write("\r\n");
      out.write("            </tr>\r\n");
      out.write("          </tbody>\r\n");
      out.write("          ");

                  }
                }
              }
            }
          
      out.write("</table>\r\n");
      out.write("      </td></tr>\r\n");
      out.write("      <tr>\r\n");
      out.write("        <td bgcolor=\"#FFFFFF\"><img border=\"0\" src=\"images/bluestripesbottom.gif\"/></td>\r\n");
      out.write("        <td align=\"right\" bgcolor=\"#FFFFFF\"><img border=\"0\" src=\"images/bluestripesbottomright.gif\"/></td>\r\n");
      out.write("      </tr>\r\n");
      out.write("      <tr><td colspan=\"2\">&nbsp;</td></tr>\r\n");
      out.write("    </tbody>\r\n");
      out.write("  </table>\r\n");
      out.write("  <td>&nbsp;</td>\r\n");
      out.write("\r\n");
      out.write("</tr>\r\n");
      out.write("<tr>\r\n");
      out.write("<!----------------------------------------------------------------------------->\r\n");
      out.write("<!------------------------- FOURTH COMPILER ----------------------------------->\r\n");
      out.write("<!----------------------------------------------------------------------------->\r\n");
      out.write("<!----------------------------------------------------------------------------->\r\n");
      out.write("<!-------------------------------  COMPILER cw10 ---------------------->\r\n");
      out.write("<!----------------------------------------------------------------------------->\r\n");
      out.write(" <td><h4 class=\"fsbolero\" align=\"center\" >cw10</h4>\r\n");
      out.write("  <table align=\"center\" border=\"0\" cellpadding=\"0\" cellspacing=\"0\">\r\n");
      out.write("    <tbody>\r\n");
      out.write("      <tr><td colspan=\"2\">&nbsp;</td></tr>\r\n");
      out.write("      <tr>\r\n");
      out.write("        <td class=\"header-row\"><img border=\"0\" src=\"images/bluestripestop.gif\"/></td>\r\n");
      out.write("        <td class=\"header-row\" align=\"right\"><img border=\"0\" src=\"images/bluestripestopright.gif\"/></td>\r\n");
      out.write("      </tr>\r\n");
      out.write("      <tr><td colspan=\"2\">\r\n");
      out.write("        <table class=\"index\" width=\"100%\" cellpadding=\"0\" cellspacing=\"1\">\r\n");
      out.write("          ");

            logDirPath = application.getInitParameter("logDir");
            if (logDirPath == null) {
          
      out.write("<tr><td>You need to provide a value for the context parameter <code>&quot;logDir&quot;</code></td></tr>");

        }
        else {
          java.io.File logDir_cw10 = new java.io.File(logDirPath);
          if (logDir_cw10.isDirectory() == false) {
        
      out.write("<tr><td>Context parameter logDir needs to be set to a directory. Currently set to &quot;");
      out.print(logDirPath);
      out.write("\r\n");
      out.write("          &quot;</td></tr>");

        }
        else {
          String[] projectDirs = logDir_cw10.list(new java.io.FilenameFilter() {
            public boolean accept(File dir, String filename) {
               boolean fileOK = true;
				fileOK &= filename.endsWith("cw10");
				fileOK &= filename.startsWith("ar_int_");
               return fileOK;
            }
          });
          if (projectDirs.length == 0) {
        
      out.write("<tr><td>no project directories found under ");
      out.print(logDirPath);
      out.write("</td></tr>");

        }
        else {
        
      out.write(" <thead class=\"index-header\">\r\n");
      out.write("          <tr class=\"header-row\">\r\n");
      out.write("\t\t\t<td  class=\"project\">Module</td>\r\n");
      out.write("\t\t\t<td  class=\"project\">Project name</td>\r\n");
      out.write("            <td  class=\"status\">Last build</td>\r\n");
      out.write("            <td  class=\"data\">Label</td>\r\n");
      out.write("            <td  class=\"project\">Tests/Failed</td>\r\n");
      out.write("        </tr>\r\n");
      out.write("        </thead>\r\n");
      out.write("          <tbody>\r\n");
      out.write("            ");

              Info[] info = new Info[projectDirs.length];
              for (int i = 0; i < info.length; i++) {
                info[i] = new Info(logDir_cw10, projectDirs[i]);
              }
              Comparator order = (Comparator) sortOrders.get(sort);
              if (order == null) {
              }
              else {
                Arrays.sort(info, order);
              }

              for (int i = 0; i < info.length; i++) {
				 TestInfo testInfo = new TestInfo(info[i].project);
				 String color_green=(i % 2 == 1) ? "#22CC22" : "#11AA11";
				 String color_red=(i % 2 == 1) ? "#FF2222" : "#CC1111";
				 String module_name_color=(i % 2 == 1) ? "#FFFFAA" : "#CCCCAA";
				 String bgcolor_class=(i % 2 == 1) ? "even-row" : "odd-row";
				 if (info[i].getStatus().toString().equals("building") || info[i].getStatus().toString().equals("publishing")){
					bgcolor_class="building-row";
					color_green="";
					color_red="";
					module_name_color="";
				 }
				 
				 String moduleName=info[i].project.replaceAll("ar_int_","");
				 moduleName=moduleName.replaceAll("_cw10","");
            
      out.write("\r\n");
      out.write("\t\t<tr class=\"");
      out.print( bgcolor_class );
      out.write(" \">\r\n");
      out.write("              <td  rowspan=\"2\" align=\"center\" bgcolor=\"");
      out.print( module_name_color );
      out.write(" \" ><b>");
      out.print(moduleName);
      out.write("</b></td>\r\n");
      out.write("\t\t\t  <td  class=\"data\"><a href=\"buildresults/");
      out.print(info[i].project);
      out.write('"');
      out.write('>');
      out.print(info[i].project);
      out.write("</a></td>\r\n");
      out.write("              <td  class=\"");
      out.print( (info[i].failed()) ? "data date failure" : "data date" );
      out.write('"');
      out.write('>');
      out.print( (info[i].failed()) ? info[i].getLastBuildTime() : info[i].getLastSuccessfulBuildTime() );
      out.write("</td>\r\n");
      out.write("              <td  class=\"data\">");
      out.print( info[i].getLabel());
      out.write("</td>\r\n");
      out.write("              <td  rowspan=\"2\" align=\"center\" bgcolor=");
      out.print( (testInfo.getPass()) ? color_green : color_red );
      out.write('>');
      out.print( testInfo.getTotal() );
      out.write(' ');
      out.write('/');
      out.write(' ');
      out.print( testInfo.getFailed() );
      out.write("\r\n");
      out.write("\t\t<tr class=\"");
      out.print( bgcolor_class );
      out.write(" \">\r\n");
      out.write("              <td align=\"center\" colspan=2 class=\"data date status-");
      out.print( info[i].getStatus().getImportance() );
      out.write("\"><small>");
      out.print( info[i].getStatus());
      out.write(" <em>(");
      out.print( info[i].getStatusSince() );
      out.write(")</em></small></td>\r\n");
      out.write("              ");
 if (jmxEnabled) { 
      out.write("\r\n");
      out.write("              <td class=\"data\"><!-- <input id=\"");
      out.print( "force_" + info[i].project );
      out.write("\" type=\"button\"\r\n");
      out.write("                                      onclick=\"callServer('");
      out.print( jmxURLPrefix.toExternalForm() + info[i].project );
      out.write("', '");
      out.print(info[i].project);
      out.write("')\"\r\n");
      out.write("                                      value=\"Build\"/> --></td>\r\n");
      out.write("              ");
 } 
      out.write("\r\n");
      out.write("            </tr>\r\n");
      out.write("          </tbody>\r\n");
      out.write("          ");

                  }
                }
              }
            }
          
      out.write("</table>\r\n");
      out.write("      </td></tr>\r\n");
      out.write("      <tr>\r\n");
      out.write("        <td bgcolor=\"#FFFFFF\"><img border=\"0\" src=\"images/bluestripesbottom.gif\"/></td>\r\n");
      out.write("        <td align=\"right\" bgcolor=\"#FFFFFF\"><img border=\"0\" src=\"images/bluestripesbottomright.gif\"/></td>\r\n");
      out.write("      </tr>\r\n");
      out.write("      <tr><td colspan=\"2\">&nbsp;</td></tr>\r\n");
      out.write("    </tbody>\r\n");
      out.write("  </table>\r\n");
      out.write("  <td>&nbsp;</td>\r\n");
      out.write("\r\n");
      out.write("</tr>\r\n");
      out.write("<tr>\r\n");
      out.write("<!----------------------------------------------------------------------------->\r\n");
      out.write("<!------------------------- FIFTH COMPILER ------------------------------------>\r\n");
      out.write("<!----------------------------------------------------------------------------->\r\n");
      out.write("<!----------------------------------------------------------------------------->\r\n");
      out.write("<!-------------------------------  COMPILER cw88 ---------------------->\r\n");
      out.write("<!----------------------------------------------------------------------------->\r\n");
      out.write(" <td><h4 class=\"fsbolero\" align=\"center\" >cw88</h4>\r\n");
      out.write("  <table align=\"center\" border=\"0\" cellpadding=\"0\" cellspacing=\"0\">\r\n");
      out.write("    <tbody>\r\n");
      out.write("      <tr><td colspan=\"2\">&nbsp;</td></tr>\r\n");
      out.write("      <tr>\r\n");
      out.write("        <td class=\"header-row\"><img border=\"0\" src=\"images/bluestripestop.gif\"/></td>\r\n");
      out.write("        <td class=\"header-row\" align=\"right\"><img border=\"0\" src=\"images/bluestripestopright.gif\"/></td>\r\n");
      out.write("      </tr>\r\n");
      out.write("      <tr><td colspan=\"2\">\r\n");
      out.write("        <table class=\"index\" width=\"100%\" cellpadding=\"0\" cellspacing=\"1\">\r\n");
      out.write("          ");

            logDirPath = application.getInitParameter("logDir");
            if (logDirPath == null) {
          
      out.write("<tr><td>You need to provide a value for the context parameter <code>&quot;logDir&quot;</code></td></tr>");

        }
        else {
          java.io.File logDir_cw88 = new java.io.File(logDirPath);
          if (logDir_cw10.isDirectory() == false) {
        
      out.write("<tr><td>Context parameter logDir needs to be set to a directory. Currently set to &quot;");
      out.print(logDirPath);
      out.write("\r\n");
      out.write("          &quot;</td></tr>");

        }
        else {
          String[] projectDirs = logDir_cw10.list(new java.io.FilenameFilter() {
            public boolean accept(File dir, String filename) {
               boolean fileOK = true;
				fileOK &= filename.endsWith("cw88");
				fileOK &= filename.startsWith("ar_int_");
               return fileOK;
            }
          });
          if (projectDirs.length == 0) {
        
      out.write("<tr><td>no project directories found under ");
      out.print(logDirPath);
      out.write("</td></tr>");

        }
        else {
        
      out.write(" <thead class=\"index-header\">\r\n");
      out.write("          <tr class=\"header-row\">\r\n");
      out.write("\t\t\t<td  class=\"project\">Module</td>\r\n");
      out.write("\t\t\t<td  class=\"project\">Project name</td>\r\n");
      out.write("            <td  class=\"status\">Last build</td>\r\n");
      out.write("            <td  class=\"data\">Label</td>\r\n");
      out.write("            <td  class=\"project\">Tests/Failed</td>\r\n");
      out.write("        </tr>\r\n");
      out.write("        </thead>\r\n");
      out.write("          <tbody>\r\n");
      out.write("            ");

              Info[] info = new Info[projectDirs.length];
              for (int i = 0; i < info.length; i++) {
                info[i] = new Info(logDir_cw88, projectDirs[i]);
              }
              Comparator order = (Comparator) sortOrders.get(sort);
              if (order == null) {
              }
              else {
                Arrays.sort(info, order);
              }

              for (int i = 0; i < info.length; i++) {
				 TestInfo testInfo = new TestInfo(info[i].project);
				 String color_green=(i % 2 == 1) ? "#22CC22" : "#11AA11";
				 String color_red=(i % 2 == 1) ? "#FF2222" : "#CC1111";
				 String module_name_color=(i % 2 == 1) ? "#FFFFAA" : "#CCCCAA";
				 String bgcolor_class=(i % 2 == 1) ? "even-row" : "odd-row";
				 if (info[i].getStatus().toString().equals("building") || info[i].getStatus().toString().equals("publishing")){
					bgcolor_class="building-row";
					color_green="";
					color_red="";
					module_name_color="";
				 }
				 
				 String moduleName=info[i].project.replaceAll("ar_int_","");
				 moduleName=moduleName.replaceAll("_cw88","");
            
      out.write("\r\n");
      out.write("\t\t<tr class=\"");
      out.print( bgcolor_class );
      out.write(" \">\r\n");
      out.write("              <td  rowspan=\"2\" align=\"center\" bgcolor=\"");
      out.print( module_name_color );
      out.write(" \" ><b>");
      out.print(moduleName);
      out.write("</b></td>\r\n");
      out.write("\t\t\t  <td  class=\"data\"><a href=\"buildresults/");
      out.print(info[i].project);
      out.write('"');
      out.write('>');
      out.print(info[i].project);
      out.write("</a></td>\r\n");
      out.write("              <td  class=\"");
      out.print( (info[i].failed()) ? "data date failure" : "data date" );
      out.write('"');
      out.write('>');
      out.print( (info[i].failed()) ? info[i].getLastBuildTime() : info[i].getLastSuccessfulBuildTime() );
      out.write("</td>\r\n");
      out.write("              <td  class=\"data\">");
      out.print( info[i].getLabel());
      out.write("</td>\r\n");
      out.write("              <td  rowspan=\"2\" align=\"center\" bgcolor=");
      out.print( (testInfo.getPass()) ? color_green : color_red );
      out.write('>');
      out.print( testInfo.getTotal() );
      out.write(' ');
      out.write('/');
      out.write(' ');
      out.print( testInfo.getFailed() );
      out.write("\r\n");
      out.write("\t\t<tr class=\"");
      out.print( bgcolor_class );
      out.write(" \">\r\n");
      out.write("              <td align=\"center\" colspan=2 class=\"data date status-");
      out.print( info[i].getStatus().getImportance() );
      out.write("\"><small>");
      out.print( info[i].getStatus());
      out.write(" <em>(");
      out.print( info[i].getStatusSince() );
      out.write(")</em></small></td>\r\n");
      out.write("              ");
 if (jmxEnabled) { 
      out.write("\r\n");
      out.write("              <td class=\"data\"><!-- <input id=\"");
      out.print( "force_" + info[i].project );
      out.write("\" type=\"button\"\r\n");
      out.write("                                      onclick=\"callServer('");
      out.print( jmxURLPrefix.toExternalForm() + info[i].project );
      out.write("', '");
      out.print(info[i].project);
      out.write("')\"\r\n");
      out.write("                                      value=\"Build\"/> --></td>\r\n");
      out.write("              ");
 } 
      out.write("\r\n");
      out.write("            </tr>\r\n");
      out.write("          </tbody>\r\n");
      out.write("          ");

                  }
                }
              }
            }
          
      out.write("</table>\r\n");
      out.write("      </td></tr>\r\n");
      out.write("      <tr>\r\n");
      out.write("        <td bgcolor=\"#FFFFFF\"><img border=\"0\" src=\"images/bluestripesbottom.gif\"/></td>\r\n");
      out.write("        <td align=\"right\" bgcolor=\"#FFFFFF\"><img border=\"0\" src=\"images/bluestripesbottomright.gif\"/></td>\r\n");
      out.write("      </tr>\r\n");
      out.write("      <tr><td colspan=\"2\">&nbsp;</td></tr>\r\n");
      out.write("    </tbody>\r\n");
      out.write("  </table>\r\n");
      out.write("  <td>&nbsp;</td>\r\n");
      out.write("\r\n");
      out.write("</tr>\r\n");
      out.write("</table>\r\n");
      out.write("\r\n");
      out.write("</table>\r\n");
      out.write("\r\n");
      out.write("<h2 class=\"white\" align=\"center\">Platform keeper</h2>\r\n");
      out.write("<table align=\"center\" border=\"0\" cellpadding=\"0\" cellspacing=\"0\" >\r\n");
      out.write("<tr>\r\n");
      out.write("<!----------------------------------------------------------------------------->\r\n");
      out.write("<!------------------------- PLATFORM KEEPER ----------------------------------->\r\n");
      out.write("<!----------------------------------------------------------------------------->\r\n");
      out.write("<!----------------------------------------------------------------------------->\r\n");
      out.write("<!-------------------------------  COMPILER keeper/updater ---------------------->\r\n");
      out.write("<!----------------------------------------------------------------------------->\r\n");
      out.write(" <td><h4 class=\"fsbolero\" align=\"center\" >keeper</h4>\r\n");
      out.write("  <table align=\"center\" border=\"0\" cellpadding=\"0\" cellspacing=\"0\">\r\n");
      out.write("    <tbody>\r\n");
      out.write("      <tr><td colspan=\"2\">&nbsp;</td></tr>\r\n");
      out.write("      <tr>\r\n");
      out.write("        <td class=\"header-row\"><img border=\"0\" src=\"images/bluestripestop.gif\"/></td>\r\n");
      out.write("        <td class=\"header-row\" align=\"right\"><img border=\"0\" src=\"images/bluestripestopright.gif\"/></td>\r\n");
      out.write("      </tr>\r\n");
      out.write("      <tr><td colspan=\"2\">\r\n");
      out.write("        <table class=\"index\" width=\"100%\" cellpadding=\"0\" cellspacing=\"1\">\r\n");
      out.write("          ");

            logDirPath = application.getInitParameter("logDir");
            if (logDirPath == null) {
          
      out.write("<tr><td>You need to provide a value for the context parameter <code>&quot;logDir&quot;</code></td></tr>");

        }
        else {
          java.io.File logDir_keeper = new java.io.File(logDirPath);
          if (logDir_keeper.isDirectory() == false) {
        
      out.write("<tr><td>Context parameter logDir needs to be set to a directory. Currently set to &quot;");
      out.print(logDirPath);
      out.write("\r\n");
      out.write("          &quot;</td></tr>");

        }
        else {
          String[] projectDirs = logDir_keeper.list(new java.io.FilenameFilter() {
            public boolean accept(File dir, String filename) {
               boolean fileOK = true;
				fileOK &= filename.endsWith("keeper");
				fileOK &= filename.startsWith("ar_int_");
               return fileOK;
            }
          });
          if (projectDirs.length == 0) {
        
      out.write("<tr><td>no project directories found under ");
      out.print(logDirPath);
      out.write("</td></tr>");

        }
        else {
        
      out.write(" <thead class=\"index-header\">\r\n");
      out.write("          <tr class=\"header-row\">\r\n");
      out.write("\t\t\t<td  class=\"project\">Module</td>\r\n");
      out.write("\t\t\t<td  class=\"project\">Project name</td>\r\n");
      out.write("            <td  class=\"status\">Last build</td>\r\n");
      out.write("            <td  class=\"data\">Label</td>\r\n");
      out.write("            <td  class=\"project\">Tests/Failed</td>\r\n");
      out.write("        </tr>\r\n");
      out.write("        </thead>\r\n");
      out.write("          <tbody>\r\n");
      out.write("            ");

              Info[] info = new Info[projectDirs.length];
              for (int i = 0; i < info.length; i++) {
                info[i] = new Info(logDir_keeper, projectDirs[i]);
              }
              Comparator order = (Comparator) sortOrders.get(sort);
              if (order == null) {
              }
              else {
                Arrays.sort(info, order);
              }

              for (int i = 0; i < info.length; i++) {
				 TestInfo testInfo = new TestInfo(info[i].project);
				 String color_green=(i % 2 == 1) ? "#22CC22" : "#11AA11";
				 String color_red=(i % 2 == 1) ? "#FF2222" : "#CC1111";
				 String module_name_color=(i % 2 == 1) ? "#FFFFAA" : "#CCCCAA";
				 String bgcolor_class=(i % 2 == 1) ? "even-row" : "odd-row";
				 if (info[i].getStatus().toString().equals("building") || info[i].getStatus().toString().equals("publishing")){
					bgcolor_class="building-row";
					color_green="";
					color_red="";
					module_name_color="";
				 }
				 
				 String moduleName=info[i].project.replaceAll("ar_int_","");
				 moduleName=moduleName.replaceAll("_keeper","");
            
      out.write("\r\n");
      out.write("\t\t<tr class=\"");
      out.print( bgcolor_class );
      out.write(" \">\r\n");
      out.write("              <td  rowspan=\"2\" align=\"center\" bgcolor=\"");
      out.print( module_name_color );
      out.write(" \" ><b>");
      out.print(moduleName);
      out.write("</b></td>\r\n");
      out.write("\t\t\t  <td  class=\"data\"><a href=\"buildresults/");
      out.print(info[i].project);
      out.write('"');
      out.write('>');
      out.print(info[i].project);
      out.write("</a></td>\r\n");
      out.write("              <td  class=\"");
      out.print( (info[i].failed()) ? "data date failure" : "data date" );
      out.write('"');
      out.write('>');
      out.print( (info[i].failed()) ? info[i].getLastBuildTime() : info[i].getLastSuccessfulBuildTime() );
      out.write("</td>\r\n");
      out.write("              <td  class=\"data\">");
      out.print( info[i].getLabel());
      out.write("</td>\r\n");
      out.write("              <td  rowspan=\"2\" align=\"center\" bgcolor=");
      out.print( (testInfo.getPass()) ? color_green : color_red );
      out.write('>');
      out.print( testInfo.getTotal() );
      out.write(' ');
      out.write('/');
      out.write(' ');
      out.print( testInfo.getFailed() );
      out.write("\r\n");
      out.write("\t\t<tr class=\"");
      out.print( bgcolor_class );
      out.write(" \">\r\n");
      out.write("              <td align=\"center\" colspan=2 class=\"data date status-");
      out.print( info[i].getStatus().getImportance() );
      out.write("\"><small>");
      out.print( info[i].getStatus());
      out.write(" <em>(");
      out.print( info[i].getStatusSince() );
      out.write(")</em></small></td>\r\n");
      out.write("              ");
 if (jmxEnabled) { 
      out.write("\r\n");
      out.write("              <td class=\"data\"><!-- <input id=\"");
      out.print( "force_" + info[i].project );
      out.write("\" type=\"button\"\r\n");
      out.write("                                      onclick=\"callServer('");
      out.print( jmxURLPrefix.toExternalForm() + info[i].project );
      out.write("', '");
      out.print(info[i].project);
      out.write("')\"\r\n");
      out.write("                                      value=\"Build\"/> --></td>\r\n");
      out.write("              ");
 } 
      out.write("\r\n");
      out.write("            </tr>\r\n");
      out.write("          </tbody>\r\n");
      out.write("          ");

                  }
                }
              }
            }
          
      out.write("</table>\r\n");
      out.write("      </td></tr>\r\n");
      out.write("      <tr>\r\n");
      out.write("        <td bgcolor=\"#FFFFFF\"><img border=\"0\" src=\"images/bluestripesbottom.gif\"/></td>\r\n");
      out.write("        <td align=\"right\" bgcolor=\"#FFFFFF\"><img border=\"0\" src=\"images/bluestripesbottomright.gif\"/></td>\r\n");
      out.write("      </tr>\r\n");
      out.write("      <tr><td colspan=\"2\">&nbsp;</td></tr>\r\n");
      out.write("    </tbody>\r\n");
      out.write("  </table>\r\n");
      out.write("  <td>&nbsp;</td>\r\n");
      out.write("\r\n");
      out.write("</tr>\r\n");
      out.write("</table>\r\n");
      out.write("\r\n");
      out.write("    <tfoot>\r\n");
      out.write("        <tr>\r\n");
      out.write("            <td colspan=\"2\" align=\"right\"><a href=\"rss\"><img border=\"0\" src=\"images/rss.png\"/></a></td>\r\n");
      out.write("        </tr>\r\n");
      out.write("    </tfoot>\r\n");
      out.write("  </table>\r\n");
      out.write("</form>\r\n");
      out.write("</body>\r\n");
      out.write("</html>\r\n");
      out.write("\r\n");
    } catch (Throwable t) {
      if (!(t instanceof SkipPageException)){
        out = _jspx_out;
        if (out != null && out.getBufferSize() != 0)
          out.clearBuffer();
        if (_jspx_page_context != null) _jspx_page_context.handlePageException(t);
      }
    } finally {
      if (_jspxFactory != null) _jspxFactory.releasePageContext(_jspx_page_context);
    }
  }
}
