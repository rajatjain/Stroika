StroikaRoot=$(abspath ../../../../../)/

include $(StroikaRoot)IntermediateFiles/$(CONFIGURATION)/Configuration.mk

SrcDir	=   $(StroikaRoot)Library/Sources/Stroika/Foundation/Debug/
ObjDir	=	$(StroikaRoot)IntermediateFiles/$(CONFIGURATION)/Library/Foundation/Debug/

include $(StroikaRoot)ScriptsLib/Makefile-Common.mk
include $(StroikaRoot)Library/Projects/Unix/SharedBuildRules-Default.mk
include $(StroikaRoot)Library/Projects/Unix/SharedMakeVariables-Default.mk


vpath %cpp $(SrcDir)


Objs	=	\
	$(ObjDir)Assertions${OBJ_SUFFIX}\
	$(ObjDir)AssertExternallySynchronizedLock${OBJ_SUFFIX}\
	$(ObjDir)BackTrace${OBJ_SUFFIX}\
	$(ObjDir)Debugger${OBJ_SUFFIX}\
	$(ObjDir)Demangle${OBJ_SUFFIX}\
	$(ObjDir)Fatal${OBJ_SUFFIX}\
	$(ObjDir)MallocGuard${OBJ_SUFFIX}\
	$(ObjDir)Trace${OBJ_SUFFIX}\


all:	$(Objs)