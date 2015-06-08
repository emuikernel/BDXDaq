BIN_DIR    = $(CLON_BIN)

JAVAC = $(JAVA_HOME)/bin/javac -classpath $(CLON)/share/jar/ss.jar

#JAVA2_DIR  =  /apps/java/jdk1.2.1/bin/
#JAVA1_DIR  =  /apps/java/jdk1.2.1/bin/

all: 
	$(JAVAC) ss_receive.java; \
	$(JAVAC) ECLaserMon.java 


exports: update
export: update
install: update 


clean:
	rm -f *.class


update:
	@echo "gui update: copy xxx.class to $CLON_BIN" 
	cp *.class  $(BIN_DIR)/


distclean:
	rm -f *.class
