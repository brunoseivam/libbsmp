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
FINAL_IMAGE=dist/${CND_CONF}/${IMAGE_TYPE}/libbsmp.X.${OUTPUT_SUFFIX}
else
IMAGE_TYPE=production
OUTPUT_SUFFIX=a
DEBUGGABLE_SUFFIX=
FINAL_IMAGE=dist/${CND_CONF}/${IMAGE_TYPE}/libbsmp.X.${OUTPUT_SUFFIX}
endif

# Object Directory
OBJECTDIR=build/${CND_CONF}/${IMAGE_TYPE}

# Distribution Directory
DISTDIR=dist/${CND_CONF}/${IMAGE_TYPE}

# Source Files Quoted if spaced
SOURCEFILES_QUOTED_IF_SPACED=../../../src/md5/md5.c ../../../src/bsmp.c ../../../src/client.c ../../../src/server.c ../../../src/server_priv.c

# Object Files Quoted if spaced
OBJECTFILES_QUOTED_IF_SPACED=${OBJECTDIR}/_ext/534552796/md5.o ${OBJECTDIR}/_ext/1386528437/bsmp.o ${OBJECTDIR}/_ext/1386528437/client.o ${OBJECTDIR}/_ext/1386528437/server.o ${OBJECTDIR}/_ext/1386528437/server_priv.o
POSSIBLE_DEPFILES=${OBJECTDIR}/_ext/534552796/md5.o.d ${OBJECTDIR}/_ext/1386528437/bsmp.o.d ${OBJECTDIR}/_ext/1386528437/client.o.d ${OBJECTDIR}/_ext/1386528437/server.o.d ${OBJECTDIR}/_ext/1386528437/server_priv.o.d

# Object Files
OBJECTFILES=${OBJECTDIR}/_ext/534552796/md5.o ${OBJECTDIR}/_ext/1386528437/bsmp.o ${OBJECTDIR}/_ext/1386528437/client.o ${OBJECTDIR}/_ext/1386528437/server.o ${OBJECTDIR}/_ext/1386528437/server_priv.o

# Source Files
SOURCEFILES=../../../src/md5/md5.c ../../../src/bsmp.c ../../../src/client.c ../../../src/server.c ../../../src/server_priv.c


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
	${MAKE}  -f nbproject/Makefile-default.mk dist/${CND_CONF}/${IMAGE_TYPE}/libbsmp.X.${OUTPUT_SUFFIX}

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
${OBJECTDIR}/_ext/534552796/md5.o: ../../../src/md5/md5.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/534552796 
	@${RM} ${OBJECTDIR}/_ext/534552796/md5.o.d 
	@${RM} ${OBJECTDIR}/_ext/534552796/md5.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/534552796/md5.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1 -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/534552796/md5.o.d" -o ${OBJECTDIR}/_ext/534552796/md5.o ../../../src/md5/md5.c   
	
${OBJECTDIR}/_ext/1386528437/bsmp.o: ../../../src/bsmp.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1386528437 
	@${RM} ${OBJECTDIR}/_ext/1386528437/bsmp.o.d 
	@${RM} ${OBJECTDIR}/_ext/1386528437/bsmp.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1386528437/bsmp.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1 -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1386528437/bsmp.o.d" -o ${OBJECTDIR}/_ext/1386528437/bsmp.o ../../../src/bsmp.c   
	
${OBJECTDIR}/_ext/1386528437/client.o: ../../../src/client.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1386528437 
	@${RM} ${OBJECTDIR}/_ext/1386528437/client.o.d 
	@${RM} ${OBJECTDIR}/_ext/1386528437/client.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1386528437/client.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1 -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1386528437/client.o.d" -o ${OBJECTDIR}/_ext/1386528437/client.o ../../../src/client.c   
	
${OBJECTDIR}/_ext/1386528437/server.o: ../../../src/server.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1386528437 
	@${RM} ${OBJECTDIR}/_ext/1386528437/server.o.d 
	@${RM} ${OBJECTDIR}/_ext/1386528437/server.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1386528437/server.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1 -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1386528437/server.o.d" -o ${OBJECTDIR}/_ext/1386528437/server.o ../../../src/server.c   
	
${OBJECTDIR}/_ext/1386528437/server_priv.o: ../../../src/server_priv.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1386528437 
	@${RM} ${OBJECTDIR}/_ext/1386528437/server_priv.o.d 
	@${RM} ${OBJECTDIR}/_ext/1386528437/server_priv.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1386528437/server_priv.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1 -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1386528437/server_priv.o.d" -o ${OBJECTDIR}/_ext/1386528437/server_priv.o ../../../src/server_priv.c   
	
else
${OBJECTDIR}/_ext/534552796/md5.o: ../../../src/md5/md5.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/534552796 
	@${RM} ${OBJECTDIR}/_ext/534552796/md5.o.d 
	@${RM} ${OBJECTDIR}/_ext/534552796/md5.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/534552796/md5.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/534552796/md5.o.d" -o ${OBJECTDIR}/_ext/534552796/md5.o ../../../src/md5/md5.c   
	
${OBJECTDIR}/_ext/1386528437/bsmp.o: ../../../src/bsmp.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1386528437 
	@${RM} ${OBJECTDIR}/_ext/1386528437/bsmp.o.d 
	@${RM} ${OBJECTDIR}/_ext/1386528437/bsmp.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1386528437/bsmp.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1386528437/bsmp.o.d" -o ${OBJECTDIR}/_ext/1386528437/bsmp.o ../../../src/bsmp.c   
	
${OBJECTDIR}/_ext/1386528437/client.o: ../../../src/client.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1386528437 
	@${RM} ${OBJECTDIR}/_ext/1386528437/client.o.d 
	@${RM} ${OBJECTDIR}/_ext/1386528437/client.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1386528437/client.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1386528437/client.o.d" -o ${OBJECTDIR}/_ext/1386528437/client.o ../../../src/client.c   
	
${OBJECTDIR}/_ext/1386528437/server.o: ../../../src/server.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1386528437 
	@${RM} ${OBJECTDIR}/_ext/1386528437/server.o.d 
	@${RM} ${OBJECTDIR}/_ext/1386528437/server.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1386528437/server.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1386528437/server.o.d" -o ${OBJECTDIR}/_ext/1386528437/server.o ../../../src/server.c   
	
${OBJECTDIR}/_ext/1386528437/server_priv.o: ../../../src/server_priv.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1386528437 
	@${RM} ${OBJECTDIR}/_ext/1386528437/server_priv.o.d 
	@${RM} ${OBJECTDIR}/_ext/1386528437/server_priv.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1386528437/server_priv.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1386528437/server_priv.o.d" -o ${OBJECTDIR}/_ext/1386528437/server_priv.o ../../../src/server_priv.c   
	
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: compileCPP
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
else
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: archive
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
dist/${CND_CONF}/${IMAGE_TYPE}/libbsmp.X.${OUTPUT_SUFFIX}: ${OBJECTFILES}  nbproject/Makefile-${CND_CONF}.mk    
	@${MKDIR} dist/${CND_CONF}/${IMAGE_TYPE} 
	${MP_AR} $(MP_EXTRA_AR_PRE) r dist/${CND_CONF}/${IMAGE_TYPE}/libbsmp.X.${OUTPUT_SUFFIX} ${OBJECTFILES_QUOTED_IF_SPACED}    
else
dist/${CND_CONF}/${IMAGE_TYPE}/libbsmp.X.${OUTPUT_SUFFIX}: ${OBJECTFILES}  nbproject/Makefile-${CND_CONF}.mk   
	@${MKDIR} dist/${CND_CONF}/${IMAGE_TYPE} 
	${MP_AR} $(MP_EXTRA_AR_PRE) r dist/${CND_CONF}/${IMAGE_TYPE}/libbsmp.X.${OUTPUT_SUFFIX} ${OBJECTFILES_QUOTED_IF_SPACED}    
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
