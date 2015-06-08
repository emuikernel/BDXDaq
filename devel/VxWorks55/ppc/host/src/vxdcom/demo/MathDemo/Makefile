# Makefile - makefile for target/src/vxdcom/MathDemo
#
# modification history
# --------------------
# 01a,28feb00,nel  created
#
# DESCRIPTION
# This file contains the makefile rules for building the MathDemo program
#
#
TGT_DIR=$(WIND_BASE)/target
BIN_DIR=$(WIND_BASE)/host/$(WIND_HOST_TYPE)/bin

CoMathDemo_ps.cpp CoMathDemo.h CoMathDemo.tlb CoMathDemo_i.c : CoMathDemo.idl
	$(BIN_DIR)/widl CoMathDemo.idl

include $(TGT_DIR)/h/make/rules.library

C++_TEMPLATE_INST = -fimplicit-templates

CoMathDemoImpl.cpp : CoMathDemoImpl.h CoMathDemo.h CoMathDemo_i.c

CoMathDemoClient.cpp : CoMathDemo.h CoMathDemo_i.c

