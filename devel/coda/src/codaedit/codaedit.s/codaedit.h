#ifdef __cplusplus
extern "C" {
#endif
Widget CodaEditor(void *topLevel,int withExit);
void EditorSelectConfig (char *confn);
void EditorSelectExp (Widget w, char *exp);
void setCompState(char *name,int state);
#ifdef __cplusplus
}
#endif
