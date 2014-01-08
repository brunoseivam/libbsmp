#
# Generated Makefile - do not edit!
#
# Edit the Makefile in the project folder instead (../Makefile). Each target
# has a -pre and a -post target defined where you can add customized code.
#
# This makefile implements configuration specific macros and targets.


# Include project Makefile
ifeq "${IGNORE_LOCAL}" "TRUE"
# do not include local makefile. User is passing all local related variables already
else
include Makefile
# Include makefile containing local settings
ifeq "$(wildcard nbproject/Makefile-local-default.mk)" "nbproject/Makefile-local-default.mk"
include nbproject/Makefile-local-default.mk
endif
endif

# Environment
MKDIR=mkdir -p
RM=rm -f 
MV=mv 
CP=cp 

# Macros
CND_CONF=default
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
IMAGE_TYPE=debug
OUTPUT_SUFFIX=a
DEBUGGABLE_SUFFIX=
FINAL_IMAGE=dist/${CND_CONF}/${IMAGE_TYPE}/libsllp.X.${OUTPUT_SUFFIX}
else
IMAGE_TYPE=production
OUTPUT_SUFFIX=a
DEBUGGABLE_SUFFIX=
FINAL_IMAGE=dist/${CND_CONF}/${IMAGE_TYPE}/libsllp.X.${OUTPUT_SUFFIX}
endif

# Object Directory
OBJECTDIR=build/${CND_CONF}/${IMAGE_TYPE}

# Distribution Directory
DISTDIR=dist/${CND_CONF}/${IMAGE_TYPE}

# Source Files Quoted if spaced
SOURCEFILES_QUOTED_IF_SPACED=../../../../src/md5/md5.c ../../../../src/sllp.c ../../../../src/sllp_client.c ../../../../src/sllp_server.c ../../../../src/curve.c ../../../../src/func.c ../../../../src/group.c ../../../../src/var.c

# Object Files Quoted if spaced
OBJECTFILES_QUOTED_IF_SPACED=${OBJECTDIR}/_ext/165194861/md5.o ${OBJECTDIR}/_ext/671464796/sllp.o ${OBJECTDIR}/_ext/671464796/sllp_client.o ${OBJECTDIR}/_ext/671464796/sllp_server.o ${OBJECTDIR}/_ext/671464796/curve.o ${OBJECTDIR}/_ext/671464796/func.o ${OBJECTDIR}/_ext/671464796/group.o ${OBJECTDIR}/_ext/671464796/var.o
POSSIBLE_DEPFILES=${OBJECTDIR}/_ext/165194861/md5.o.d ${OBJECTDIR}/_ext/671464796/sllp.o.d ${OBJECTDIR}/_ext/671464796/sllp_client.o.d ${OBJECTDIR}/_ext/671464796/sllp_server.o.d ${OBJECTDIR}/_ext/671464796/curve.o.d ${OBJECTDIR}/_ext/671464796/func.o.d ${OBJECTDIR}/_ext/671464796/group.o.d ${OBJECTDIR}/_ext/671464796/var.o.d

# Object Files
OBJECTFILES=${OBJECTDIR}/_ext/165194861/md5.o ${OBJECTDIR}/_ext/671464796/sllp.o ${OBJECTDIR}/_ext/671464796/sllp_client.o ${OBJECTDIR}/_ext/671464796/sllp_server.o ${OBJECTDIR}/_ext/671464796/curve.o ${OBJECTDIR}/_ext/671464796/func.o ${OBJECTDIR}/_ext/671464796/group.o ${OBJECTDIR}/_ext/671464796/var.o

# Source Files
SOURCEFILES=../../../../src/md5/md5.c ../../../../src/sllp.c ../../../../src/sllp_client.c ../../../../src/sllp_server.c ../../../../src/curve.c ../../../../src/func.c ../../../../src/group.c ../../../../src/var.c


CFLAGS=
ASFLAGS=
LDLIBSOPTIONS=

############# Tool locations ##########################################
# If you copy a project from one host to another, the path where the  #
# compiler is installed may be different.                             #
# If you open this project with MPLAB X in the new host, this         #
# makefile will be regenerated and the paths will be corrected.       #
#######################################################################
# fixDeps replaces a bunch of sed/cat/printf statements that slow down the build
FIXDEPS=fixDeps

.build-conf:  ${BUILD_SUBPROJECTS}
	${MAKE}  -f nbproject/Makefile-default.mk dist/${CND_CONF}/${IMAGE_TYPE}/libsllp.X.${OUTPUT_SUFFIX}

MP_PROCESSOR_OPTION=32MX795F512L
MP_LINKER_FILE_OPTION=
# ------------------------------------------------------------------------------------
# Rules for buildStep: assemble
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
else
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: assembleWithPreprocess
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
else
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: compile
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
${OBJECTDIR}/_ext/165194861/md5.o: ../../../../src/md5/md5.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/165194861 
	@${RM} ${OBJECTDIR}/_ext/165194861/md5.o.d 
	@${RM} ${OBJECTDIR}/_ext/165194861/md5.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/165194861/md5.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1 -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -Wall -MMD -MF "${OBJECTDIR}/_ext/165194861/md5.o.d" -o ${OBJECTDIR}/_ext/165194861/md5.o ../../../../src/md5/md5.c   
	
${OBJECTDIR}/_ext/671464796/sllp.o: ../../../../src/sllp.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/671464796 
	@${RM} ${OBJECTDIR}/_ext/671464796/sllp.o.d 
	@${RM} ${OBJECTDIR}/_ext/671464796/sllp.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/671464796/sllp.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1 -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -Wall -MMD -MF "${OBJECTDIR}/_ext/671464796/sllp.o.d" -o ${OBJECTDIR}/_ext/671464796/sllp.o ../../../../src/sllp.c   
	
${OBJECTDIR}/_ext/671464796/sllp_client.o: ../../../../src/sllp_client.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/671464796 
	@${RM} ${OBJECTDIR}/_ext/671464796/sllp_client.o.d 
	@${RM} ${OBJECTDIR}/_ext/671464796/sllp_client.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/671464796/sllp_client.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1 -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -Wall -MMD -MF "${OBJECTDIR}/_ext/671464796/sllp_client.o.d" -o ${OBJECTDIR}/_ext/671464796/sllp_client.o ../../../../src/sllp_client.c   
	
${OBJECTDIR}/_ext/671464796/sllp_server.o: ../../../../src/sllp_server.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/671464796 
	@${RM} ${OBJECTDIR}/_ext/671464796/sllp_server.o.d 
	@${RM} ${OBJECTDIR}/_ext/671464796/sllp_server.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/671464796/sllp_server.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1 -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -Wall -MMD -MF "${OBJECTDIR}/_ext/671464796/sllp_server.o.d" -o ${OBJECTDIR}/_ext/671464796/sllp_server.o ../../../../src/sllp_server.c   
	
${OBJECTDIR}/_ext/671464796/curve.o: ../../../../src/curve.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/671464796 
	@${RM} ${OBJECTDIR}/_ext/671464796/curve.o.d 
	@${RM} ${OBJECTDIR}/_ext/671464796/curve.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/671464796/curve.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1 -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -Wall -MMD -MF "${OBJECTDIR}/_ext/671464796/curve.o.d" -o ${OBJECTDIR}/_ext/671464796/curve.o ../../../../src/curve.c   
	
${OBJECTDIR}/_ext/671464796/func.o: ../../../../src/func.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/671464796 
	@${RM} ${OBJECTDIR}/_ext/671464796/func.o.d 
	@${RM} ${OBJECTDIR}/_ext/671464796/func.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/671464796/func.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1 -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -Wall -MMD -MF "${OBJECTDIR}/_ext/671464796/func.o.d" -o ${OBJECTDIR}/_ext/671464796/func.o ../../../../src/func.c   
	
${OBJECTDIR}/_ext/671464796/group.o: ../../../../src/group.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/671464796 
	@${RM} ${OBJECTDIR}/_ext/671464796/group.o.d 
	@${RM} ${OBJECTDIR}/_ext/671464796/group.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/671464796/group.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1 -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -Wall -MMD -MF "${OBJECTDIR}/_ext/671464796/group.o.d" -o ${OBJECTDIR}/_ext/671464796/group.o ../../../../src/group.c   
	
${OBJECTDIR}/_ext/671464796/var.o: ../../../../src/var.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/671464796 
	@${RM} ${OBJECTDIR}/_ext/671464796/var.o.d 
	@${RM} ${OBJECTDIR}/_ext/671464796/var.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/671464796/var.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1 -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -Wall -MMD -MF "${OBJECTDIR}/_ext/671464796/var.o.d" -o ${OBJECTDIR}/_ext/671464796/var.o ../../../../src/var.c   
	
else
${OBJECTDIR}/_ext/165194861/md5.o: ../../../../src/md5/md5.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/165194861 
	@${RM} ${OBJECTDIR}/_ext/165194861/md5.o.d 
	@${RM} ${OBJECTDIR}/_ext/165194861/md5.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/165194861/md5.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -Wall -MMD -MF "${OBJECTDIR}/_ext/165194861/md5.o.d" -o ${OBJECTDIR}/_ext/165194861/md5.o ../../../../src/md5/md5.c   
	
${OBJECTDIR}/_ext/671464796/sllp.o: ../../../../src/sllp.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/671464796 
	@${RM} ${OBJECTDIR}/_ext/671464796/sllp.o.d 
	@${RM} ${OBJECTDIR}/_ext/671464796/sllp.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/671464796/sllp.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -Wall -MMD -MF "${OBJECTDIR}/_ext/671464796/sllp.o.d" -o ${OBJECTDIR}/_ext/671464796/sllp.o ../../../../src/sllp.c   
	
${OBJECTDIR}/_ext/671464796/sllp_client.o: ../../../../src/sllp_client.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/671464796 
	@${RM} ${OBJECTDIR}/_ext/671464796/sllp_client.o.d 
	@${RM} ${OBJECTDIR}/_ext/671464796/sllp_client.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/671464796/sllp_client.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -Wall -MMD -MF "${OBJECTDIR}/_ext/671464796/sllp_client.o.d" -o ${OBJECTDIR}/_ext/671464796/sllp_client.o ../../../../src/sllp_client.c   
	
${OBJECTDIR}/_ext/671464796/sllp_server.o: ../../../../src/sllp_server.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/671464796 
	@${RM} ${OBJECTDIR}/_ext/671464796/sllp_server.o.d 
	@${RM} ${OBJECTDIR}/_ext/671464796/sllp_server.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/671464796/sllp_server.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -Wall -MMD -MF "${OBJECTDIR}/_ext/671464796/sllp_server.o.d" -o ${OBJECTDIR}/_ext/671464796/sllp_server.o ../../../../src/sllp_server.c   
	
${OBJECTDIR}/_ext/671464796/curve.o: ../../../../src/curve.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/671464796 
	@${RM} ${OBJECTDIR}/_ext/671464796/curve.o.d 
	@${RM} ${OBJECTDIR}/_ext/671464796/curve.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/671464796/curve.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -Wall -MMD -MF "${OBJECTDIR}/_ext/671464796/curve.o.d" -o ${OBJECTDIR}/_ext/671464796/curve.o ../../../../src/curve.c   
	
${OBJECTDIR}/_ext/671464796/func.o: ../../../../src/func.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/671464796 
	@${RM} ${OBJECTDIR}/_ext/671464796/func.o.d 
	@${RM} ${OBJECTDIR}/_ext/671464796/func.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/671464796/func.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -Wall -MMD -MF "${OBJECTDIR}/_ext/671464796/func.o.d" -o ${OBJECTDIR}/_ext/671464796/func.o ../../../../src/func.c   
	
${OBJECTDIR}/_ext/671464796/group.o: ../../../../src/group.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/671464796 
	@${RM} ${OBJECTDIR}/_ext/671464796/group.o.d 
	@${RM} ${OBJECTDIR}/_ext/671464796/group.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/671464796/group.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -Wall -MMD -MF "${OBJECTDIR}/_ext/671464796/group.o.d" -o ${OBJECTDIR}/_ext/671464796/group.o ../../../../src/group.c   
	
${OBJECTDIR}/_ext/671464796/var.o: ../../../../src/var.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/671464796 
	@${RM} ${OBJECTDIR}/_ext/671464796/var.o.d 
	@${RM} ${OBJECTDIR}/_ext/671464796/var.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/671464796/var.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -Wall -MMD -MF "${OBJECTDIR}/_ext/671464796/var.o.d" -o ${OBJECTDIR}/_ext/671464796/var.o ../../../../src/var.c   
	
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: compileCPP
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
else
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: archive
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
dist/${CND_CONF}/${IMAGE_TYPE}/libsllp.X.${OUTPUT_SUFFIX}: ${OBJECTFILES}  nbproject/Makefile-${CND_CONF}.mk    
	@${MKDIR} dist/${CND_CONF}/${IMAGE_TYPE} 
	${MP_AR} $(MP_EXTRA_AR_PRE) r dist/${CND_CONF}/${IMAGE_TYPE}/libsllp.X.${OUTPUT_SUFFIX} ${OBJECTFILES_QUOTED_IF_SPACED}    
else
dist/${CND_CONF}/${IMAGE_TYPE}/libsllp.X.${OUTPUT_SUFFIX}: ${OBJECTFILES}  nbproject/Makefile-${CND_CONF}.mk   
	@${MKDIR} dist/${CND_CONF}/${IMAGE_TYPE} 
	${MP_AR} $(MP_EXTRA_AR_PRE) r dist/${CND_CONF}/${IMAGE_TYPE}/libsllp.X.${OUTPUT_SUFFIX} ${OBJECTFILES_QUOTED_IF_SPACED}    
endif


# Subprojects
.build-subprojects:


# Subprojects
.clean-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r build/default
	${RM} -r dist/default

# Enable dependency checking
.dep.inc: .depcheck-impl

DEPFILES=$(shell "${PATH_TO_IDE_BIN}"mplabwildcard ${POSSIBLE_DEPFILES})
ifneq (${DEPFILES},)
include ${DEPFILES}
endif
