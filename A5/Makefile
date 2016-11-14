KIND:=PAD

CXX =u++ # compiler
CXXFLAGS =-g -std=c++1y -multi -nodebug -Wall -Wno-unused-label -MMD -O2 -D${KIND}
MAKEFILE_NAME = ${firstword ${MAKEFILE_LIST}}	# makefile name

OBJECTS01 = q1.o # optional build of given program
EXEC01 = counters${KIND}

# OBJECTS2 = q2tallyVotes${TYPE}.o # list of object files for question 1 prefixed with "q2"
#
# EXEC2 = vote

OBJECTS = ${OBJECTS01} # all object files
DEPENDS = ${OBJECTS:.o=.d} # substitute ".o" with ".d"
EXECS = ${EXEC01} # all executables

#############################################################

.PHONY : all clean

all : ${EXECS}					# build all executables

q1.o : q1.cc				# add flags 1st executable, ADJUST SUFFIX (.cc)
	${CXX} ${CXXFLAGS} -nodebug -c $< -o $@

.PHONY : ${EXEC01}
${EXEC01} :
	touch q1.cc
	sleep 1
	${MAKE} ${EXEC01} KIND="${KIND}"

#############################################################

# ifeq (${IMPLTYPE},${TYPE})			# same implementation type as last time ?
# ${EXEC2} : ${OBJECTS2}
# 	${CXX} ${CXXFLAGS} $^ -o $@
# else
# ifeq (${TYPE},)					# no implementation type specified ?
# # set type to previous type
# TYPE=${IMPLTYPE}
# ${EXEC2} : ${OBJECTS2}
# 	${CXX} ${CXXFLAGS} $^ -o $@
# else						# implementation type has changed
# .PHONY : ${EXEC2}
# ${EXEC2} :
# 	rm -f ImplType
# 	touch q2tallyVotes.h
# 	sleep 1
# 	${MAKE} ${EXEC2} TYPE="${TYPE}"
# endif
# endif
#
# ImplType :
# 	echo "IMPLTYPE=${TYPE}" > ImplType
# 	sleep 1

#############################################################

${OBJECTS} : ${MAKEFILE_NAME}			# OPTIONAL : changes to this file => recompile

-include ${DEPENDS}				# include *.d files containing program dependences

clean :						# remove files that can be regenerated
	rm -f *.d *.o ${EXECS}
