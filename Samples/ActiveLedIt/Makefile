StroikaRoot=$(abspath ../../)/

ifneq ($(CONFIGURATION),)
	include $(StroikaRoot)IntermediateFiles/$(CONFIGURATION)/Configuration.mk
endif

include $(StroikaRoot)ScriptsLib/Makefile-Common.mk

SrcDir				=	$(StroikaRoot)Samples/WebServer/
ObjDir				=	$(StroikaRoot)IntermediateFiles/$(CONFIGURATION)/Samples_WebServer/

include $(StroikaRoot)Library/Projects/Unix/SharedBuildRules-Default.mk
include $(StroikaRoot)Library/Projects/Unix/SharedMakeVariables-Default.mk

TARGETEXE				=	$(StroikaRoot)Builds/$(CONFIGURATION)/Samples-ActiveLedIt/SophistSolutions.ActiveLedIt.dll


VPATH	=			$(SrcDir)/Sources/


Objs	=	\
	$(ObjDir)ActiveLedIt.o\


all:
ifeq (VisualStudio,$(findstring VisualStudio,$(ProjectPlatformSubdir)))
	@$(StroikaRoot)ScriptsLib/PrintProgressLine $(MAKE_INDENT_LEVEL) "Building Samples/ActiveLedIt:"
	@if [[ "$(CONFIGURATION)" == "Debug-U-32" || "$(CONFIGURATION)" == "Release-U-32" ]] ; then\
		$(StroikaRoot)ScriptsLib/RunArgumentsWithCommonBuildVars $(CONFIGURATION) "MSBuild.exe /nologo /v:quiet /clp:NoSummary Projects/$(ProjectPlatformSubdir)/ActiveLedIt.vcxproj /p:`$(StroikaRoot)ScriptsLib/GetVisualStudioConfigLine $(CONFIGURATION)` /target:Build";\
	else\
		$(StroikaRoot)ScriptsLib/PrintProgressLine $$(($(MAKE_INDENT_LEVEL)+1)) "SKIPPED for unsupported configuration";\
	fi
endif


check:
	@$(StroikaRoot)ScriptsLib/PrintProgressLine $$(($(MAKE_INDENT_LEVEL) + 1)) -n "Checking Samples/ActiveLedIt...              "
ifeq (VisualStudio,$(findstring VisualStudio,$(ProjectPlatformSubdir)))
	@if [[ "$(CONFIGURATION)" == "Debug-U-32" || "$(CONFIGURATION)" == "Release-U-32" ]] ; then\
		$(StroikaRoot)ScriptsLib/CheckFileExists $(TARGETEXE);\
		$(ECHO) "[SUCCEEDED]";\
	else\
		$(ECHO) "[SKIPPED]";\
	fi
else
	@$(StroikaRoot)ScriptsLib/CheckFileExists $(TARGETEXE)
	@$(ECHO) "[SUCCEEDED]";
endif


$(TARGETEXE):	$(ObjDir) $(Objs) $(StroikaLibs)
	@$(StroikaRoot)ScriptsLib/PrintProgressLine $(MAKE_INDENT_LEVEL) "Linking `$(StroikaRoot)ScriptsLib/SubstituteBackVariables $@`" "... "
	@if [ $(ECHO_BUILD_LINES) -eq 1 ]; then\
	    $(StroikaRoot)ScriptsLib/PrintProgressLine $$(($(MAKE_INDENT_LEVEL)+1)) "$(Linker) $(StroikaLinkerPrefixArgs) -o $(TARGETEXE) $(Objs) $(StroikaLinkerSuffixArgs)";\
	fi
	@$(Linker) $(StroikaLinkerPrefixArgs) -o $(TARGETEXE) $(Objs) $(StroikaLinkerSuffixArgs)


