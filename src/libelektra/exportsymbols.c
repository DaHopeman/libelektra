#include <stdio.h>
#include <string.h>

int main(int argc, char**argv)
{
	int i;

	/*Exit if no backend is given with error code, because
	  argv[1] is used below*/
	if (argc < 2) return 1;

	FILE *f = fopen("exported_symbols.h", "w");
	fprintf (f, 	"/* exported_symbols.h generated by exportsymbols \n"
			" *\n"
			" * The static case\n"
			" *\n"
			" * Struct which contain export symbols\n"
			" *  Format :\n"
			" *  --------\n"
			" *\n"
			" *  filename, NULL\n"
			" *  symbol1, &func1,\n"
			" *  symbol2, &func2,\n"
			" *  filename2, NULL\n"
			" *  symbol1, &func1,\n"
			" *  symbol2, &func2,\n"
			" *  ....\n"
			" *  symboln, &funcn,\n"
			" *  NULL, NULL\n"
			" */\n\n"

			"typedef struct {\n"
			"	const char *name;\n"
			"	void (*function)(void);\n"
			"} kdblib_symbol;\n\n"

			"extern kdblib_symbol kdb_exported_syms[];\n\n"

			);

	for (i=1; i<argc; ++i)
	{
		fprintf(f, "extern void libelektra_%s_LTX_elektraPluginSymbol (void);\n", argv[i]);
	}

	fclose (f);

	f = fopen("exported_symbols.c", "w");

	fprintf(f, 	"/* exported_symbols.c generated by exportsymbols.sh */\n\n"

			"#include <exported_symbols.h>\n\n"

			"kdblib_symbol kdb_exported_syms[] =\n"
			"{\n");

	// quite a number of tests still rely on that they can
	// load a backend called "default"
	fprintf(f, "\t{\"default\", 0},\n");
	fprintf(f, "\t{\"elektraPluginSymbol\", &libelektra_%s_LTX_elektraPluginSymbol},\n", argv[1]);

	printf ("Exporting symbols for ");
	for (i=1; i<argc; ++i)
	{
		printf ("%s ", argv[i]);
		fprintf(f, "\t{\"%s\", 0},\n", argv[i]);
		fprintf(f, "\t{\"elektraPluginSymbol\", &libelektra_%s_LTX_elektraPluginSymbol},\n", argv[i]);
	}
	printf ("\n");

	fprintf(f, "\t{ 0 , 0 }\n");
	fprintf(f, "};\n");

	fclose (f);
	return 0;
}
