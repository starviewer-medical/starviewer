#include "autotest.h"
#include "gtest/gtest.h"

/// A part dels paràmetres que QTest defineix de cada test, se n'han afegint més:
///     -saveOutputToDir <dirPath>: guarda l'output de cada testsuite a un fitxer del format <nomDelTest>.txt.
///                                 Requeriments: el directori NO ha d'existir o ha d'estar buit.
///     -testsToExecute string1 [string2 ... stringN]: executarà només els testos que el seu nom contingui algun dels strings especificats. És case insensitive.
///                                                    D'aquesta manera es permet executar un test concret o tots aquells que continguin una certa paraula, per exemple, "pacs".

TEST_MAIN
