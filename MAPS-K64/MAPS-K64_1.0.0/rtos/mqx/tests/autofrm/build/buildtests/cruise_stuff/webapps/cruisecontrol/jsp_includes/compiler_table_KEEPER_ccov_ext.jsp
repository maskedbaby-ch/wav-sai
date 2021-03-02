<!----------------------------------------------------------------------------->
<!-------------------------------  COMPILER keeper ---------------------->
<!----------------------------------------------------------------------------->
 <td><h4 class="fsbolero" align="center" >keeper</h4>
  <table align="center" border="0" cellpadding="0" cellspacing="0">
    <tbody>
      <tr><td colspan="2">&nbsp;</td></tr>
      <tr>
        <td class="header-row"><img border="0" src="images/bluestripestop.gif"/></td>
        <td class="header-row" align="right"><img border="0" src="images/bluestripestopright.gif"/></td>
      </tr>
      <tr><td colspan="2">
        <table class="index" width="100%" cellpadding="0" cellspacing="1">
          <%
            logDirPath = application.getInitParameter("logDir");
            if (logDirPath == null) {
          %><tr><td>You need to provide a value for the context parameter <code>&quot;logDir&quot;</code></td></tr><%
        }
        else {
          java.io.File logDir_keeper = new java.io.File(logDirPath);
          if (logDir_keeper.isDirectory() == false) {
        %><tr><td>Context parameter logDir needs to be set to a directory. Currently set to &quot;<%=logDirPath%>
          &quot;</td></tr><%
        }
        else {
          String[] projectDirs = logDir_keeper.list(new java.io.FilenameFilter() {
            public boolean accept(File dir, String filename) {
               boolean fileOK = true;
				fileOK &= filename.endsWith("keeper");
				fileOK &= filename.startsWith("ar_ccov_ext_");
               return fileOK;
            }
          });
          if (projectDirs.length == 0) {
        %><tr><td>no project directories found under <%=logDirPath%></td></tr><%
        }
        else {
        %> <thead class="index-header">
          <tr class="header-row">
			<td  class="project">Module</td>
			<td  class="project">Project name</td>
            <td  class="status">Last build</td>
            <td  class="data">Label</td>
            <td  class="project">Tests/Failed</td>
        </tr>
        </thead>
          <tbody>
            <%
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
				 
				 String moduleName=info[i].project.replaceAll("ar_ccov_ext_","");
				 moduleName=moduleName.replaceAll("_keeper","");
            %>
		<tr class="<%= bgcolor_class %> ">
              <td  rowspan="2" align="center" bgcolor="<%= module_name_color %> " ><b><%=moduleName%></b></td>
			  <td  class="data"><a href="buildresults/<%=info[i].project%>"><%=info[i].project%></a></td>
              <td  class="<%= (info[i].failed()) ? "data date failure" : "data date" %>"><%= (info[i].failed()) ? info[i].getLastBuildTime() : info[i].getLastSuccessfulBuildTime() %></td>
              <td  class="data"><%= info[i].getLabel()%></td>
              <td  rowspan="2" align="center" bgcolor=<%= (testInfo.getPass()) ? color_green : color_red %>><%= testInfo.getTotal() %> / <%= testInfo.getFailed() %>
		<tr class="<%= bgcolor_class %> ">
              <td align="center" colspan=2 class="data date status-<%= info[i].getStatus().getImportance() %>"><small><%= info[i].getStatus()%> <em>(<%= info[i].getStatusSince() %>)</em></small></td>
              <% if (jmxEnabled) { %>
              <td class="data"><!-- <input id="<%= "force_" + info[i].project %>" type="button"
                                      onclick="callServer('<%= jmxURLPrefix.toExternalForm() + info[i].project %>', '<%=info[i].project%>')"
                                      value="Build"/> --></td>
              <% } %>
            </tr>
          </tbody>
          <%
                  }
                }
              }
            }
          %></table>
      </td></tr>
      <tr>
        <td bgcolor="#FFFFFF"><img border="0" src="images/bluestripesbottom.gif"/></td>
        <td align="right" bgcolor="#FFFFFF"><img border="0" src="images/bluestripesbottomright.gif"/></td>
      </tr>
      <tr><td colspan="2">&nbsp;</td></tr>
    </tbody>
  </table>
  <td>&nbsp;</td>
