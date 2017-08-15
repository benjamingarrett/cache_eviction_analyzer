#
# Generated Makefile - do not edit!
#
# Edit the Makefile in the project folder instead (../Makefile). Each target
# has a -pre and a -post target defined where you can add customized code.
#
# This makefile implements configuration specific macros and targets.


# Environment
MKDIR=mkdir
CP=cp
GREP=grep
NM=nm
CCADMIN=CCadmin
RANLIB=ranlib
CC=gcc
CCC=g++
CXX=g++
FC=gfortran
AS=as

# Macros
CND_PLATFORM=GNU-Linux
CND_DLIB_EXT=so
CND_CONF=Release
CND_DISTDIR=dist
CND_BUILDDIR=build

# Include project Makefile
include Makefile

# Object Directory
OBJECTDIR=${CND_BUILDDIR}/${CND_CONF}/${CND_PLATFORM}

# Object Files
OBJECTFILES= \
	${OBJECTDIR}/_ext/c56916c5/MurmurHash32.o \
	${OBJECTDIR}/_ext/c56916c5/MurmurHash64A.o \
	${OBJECTDIR}/_ext/d6f51a9a/indexed_skiplist.o \
	${OBJECTDIR}/_ext/d6f51a9a/indexed_skiplist_long_int.o \
	${OBJECTDIR}/_ext/554cfc1b/memo.o \
	${OBJECTDIR}/_ext/554cfc1b/memo_long_int.o \
	${OBJECTDIR}/cache_eviction_analyzer.o \
	${OBJECTDIR}/main.o


# C Compiler Flags
CFLAGS=

# CC Compiler Flags
CCFLAGS=
CXXFLAGS=

# Fortran Compiler Flags
FFLAGS=

# Assembler Flags
ASFLAGS=

# Link Libraries and Options
LDLIBSOPTIONS=

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	"${MAKE}"  -f nbproject/Makefile-${CND_CONF}.mk ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/cache_eviction_analyzer

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/cache_eviction_analyzer: ${OBJECTFILES}
	${MKDIR} -p ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}
	${LINK.c} -o ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/cache_eviction_analyzer ${OBJECTFILES} ${LDLIBSOPTIONS}

${OBJECTDIR}/_ext/c56916c5/MurmurHash32.o: ../hashing/MurmurHash32.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/c56916c5
	${RM} "$@.d"
	$(COMPILE.c) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/c56916c5/MurmurHash32.o ../hashing/MurmurHash32.c

${OBJECTDIR}/_ext/c56916c5/MurmurHash64A.o: ../hashing/MurmurHash64A.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/c56916c5
	${RM} "$@.d"
	$(COMPILE.c) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/c56916c5/MurmurHash64A.o ../hashing/MurmurHash64A.c

${OBJECTDIR}/_ext/d6f51a9a/indexed_skiplist.o: ../indexed_skiplist/indexed_skiplist.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/d6f51a9a
	${RM} "$@.d"
	$(COMPILE.c) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/d6f51a9a/indexed_skiplist.o ../indexed_skiplist/indexed_skiplist.c

${OBJECTDIR}/_ext/d6f51a9a/indexed_skiplist_long_int.o: ../indexed_skiplist/indexed_skiplist_long_int.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/d6f51a9a
	${RM} "$@.d"
	$(COMPILE.c) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/d6f51a9a/indexed_skiplist_long_int.o ../indexed_skiplist/indexed_skiplist_long_int.c

${OBJECTDIR}/_ext/554cfc1b/memo.o: ../memoization/memo.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/554cfc1b
	${RM} "$@.d"
	$(COMPILE.c) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/554cfc1b/memo.o ../memoization/memo.c

${OBJECTDIR}/_ext/554cfc1b/memo_long_int.o: ../memoization/memo_long_int.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/554cfc1b
	${RM} "$@.d"
	$(COMPILE.c) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/554cfc1b/memo_long_int.o ../memoization/memo_long_int.c

${OBJECTDIR}/cache_eviction_analyzer.o: cache_eviction_analyzer.c 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.c) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/cache_eviction_analyzer.o cache_eviction_analyzer.c

${OBJECTDIR}/main.o: main.c 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.c) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/main.o main.c

# Subprojects
.build-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r ${CND_BUILDDIR}/${CND_CONF}
	${RM} ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/cache_eviction_analyzer

# Subprojects
.clean-subprojects:

# Enable dependency checking
.dep.inc: .depcheck-impl

include .dep.inc
