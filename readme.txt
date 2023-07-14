Ve složce exe lze nalézt 2 spustitelné soubory pro linux - generate_tests a tester

generate_tests slouží k vygenerování dat pro testy
tester pak tyto data vyzkouší na základě přepínačů zadaných při startu programu.

Tyto přepínače lze zobrazit přepínačem -h

Ve složce results jsou umístěny výsledky testů ve formátu csv. Obsahují také skript
merge_into_combined.ipynb, který všechny výsledky zloučí do souboru combined.csv

Ve složce src najdeme soubory se zdrojovými kódy. Jsou rozděleny do dvou složek 
1. impl - obsahuje zdrojové soubory pro spustitelné programy
2. thesis - obsahuje zdrojový kód pro závěrečnou práci ve formátu latex

Ve složce text je umístěn pdf soubor se závěrečnou prací.