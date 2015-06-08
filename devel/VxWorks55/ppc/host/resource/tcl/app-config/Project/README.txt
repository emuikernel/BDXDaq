Object relations
----------------
project:
  1 projectType (which provides build rules for the project)
  1 projectFiles
  n buildSpecs
	1 tc	(which has a type, like C++)
	    n tools
		1 defaultFlags
		n input suffixes
		1 output suffix
	1 flags per tool
	1 flags override (+/-) per file


Library relationships
---------------------

project         build          project
 type          settings         files
-------        --------        -------

wccLib          tc_*
  |               |
  |               |
prj_*        prjBuildLib      prjFileLib
  |               |              /
  \               |             /
   \              |            /
    -----------prjLib----------
 

