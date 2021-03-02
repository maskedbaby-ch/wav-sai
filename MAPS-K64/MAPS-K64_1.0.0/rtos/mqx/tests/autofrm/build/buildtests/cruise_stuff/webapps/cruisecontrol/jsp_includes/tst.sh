compilers="cwcf72 cwmcu63 iar keeper";
for compiler in $compilers; do
	compiler_name=$compiler
	prefix=ar_int_
	filename=compiler_table_${compiler}.jsp
	m4  	-DM4_PROJECT_COMPILER=$compiler	\
			-DM4_PROJECT_COMPILER_NAME=$compiler_name	\
			-DM4_PROJECT_PREFIX=$prefix \
		compiler_table.jsp.m4 >$filename

	compiler_name=$compiler
	prefix=ar_ext_
	filename=compiler_table_${compiler}_ext.jsp
	m4  	-DM4_PROJECT_COMPILER=$compiler	\
			-DM4_PROJECT_COMPILER_NAME=$compiler_name	\
			-DM4_PROJECT_PREFIX=$prefix \
		compiler_table.jsp.m4 >$filename
		
	compiler_name=$compiler
	prefix=ar_ccov_int_
	filename=compiler_table_${compiler}_ccov.jsp
	m4  	-DM4_PROJECT_COMPILER=$compiler	\
			-DM4_PROJECT_COMPILER_NAME=$compiler_name	\
			-DM4_PROJECT_PREFIX=$prefix \
		compiler_table.jsp.m4 >$filename
		
	compiler_name=$compiler
	prefix=ar_ccov_ext_
	filename=compiler_table_${compiler}_ccov_ext.jsp
	m4  	-DM4_PROJECT_COMPILER=$compiler	\
			-DM4_PROJECT_COMPILER_NAME=$compiler_name	\
			-DM4_PROJECT_PREFIX=$prefix \
		compiler_table.jsp.m4 >$filename
done;