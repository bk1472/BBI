#######################################################################
# Common Rules
#######################################################################
.PHONY: depend clean clobber


clean:
	@$(RM) $(OBJS)

clobber: clean
	@$(RMDIR) $(OBJ_DIR)
	@$(RM) $(TGT)

$(OBJ_DIR)/%.d : %.cpp
	@$(ECHO) "+++++++++ Making $(notdir $@)"
	@$(TST) -d $(OBJ_DIR) || mkdir -p $(OBJ_DIR)
	@( $(CXX) -MM $(CXXFLAGS) $<											\
	  | sed  -f $(UTIL_DIR)/mkdep.sed										\
	  | grep -v "^  \\\\"													\
	  | sed  -e "s\$(<:.cpp=.o)\$@ $(OBJ_DIR)/$(<:.cpp=.o)\g"				\
	) > $@ 																	\
	$(NULL)
#	) > $@ 2>$(MKD_ERR) || (cat $(MKD_ERR); rm -f $(MKD_ERR) $@; exit 1)	\

$(OBJ_DIR)/%.o:%.cpp
	@$(TST) -d $(OBJ_DIR) || mkdir -p $(OBJ_DIR)
	@$(CCDV) $(CXX) -c $(CXXFLAGS) -o $@ $<

$(TGT) : $(OBJS)
	@$(CCDV) $(LD) -o $@ $(OBJS)

ifeq ($(OBJS),)
INCLUDE_DEPEND	?= 0
else
INCLUDE_DEPEND	?= 1
ifneq ($(MAKECMDGOALS),)
ifneq ($(MAKECMDGOALS), depend)
INCLUDE_DEPEND	 = 0
endif
endif
endif

ifeq ($(INCLUDE_DEPEND), 1)
-include $(OBJS:.o=.d)
endif

$(CCDV): $(UTIL_DIR)/ccdv.src/ccdv.c $(UTIL_DIR)/ccdv.src/sift-warn.c
ifeq ($(MAKELEVEL), 0)
	@($(MAKE) -C $(UTIL_DIR)/ccdv.src > /dev/null 2>&1)
endif
