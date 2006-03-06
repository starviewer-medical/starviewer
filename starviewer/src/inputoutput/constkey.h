/** Aquesta classe conte el nom de les key per poder accedir a la informació guardada en el sistema
  */
  #ifndef CONSTKEY
#define CONSTKEY
#include <qstring.h>

const QString databaseRootKey("pacs/cache/sdatabasePath"); //indica on es troba la bd
const QString poolSizeKey("pacs/cache/poolSize");
const QString cacheImagePathKey("pacs/cache/imagePath");
const QString AETitleMachineKey("pacs/pacsparam/AETitle");
const QString localPortKey("pacs/pacsparam/localPort");
const QString timeoutPacsKey("pacs/pacsparam/timeout");
const QString maxConnectionsKey("pacs/pacsparam/MaxConnects");
const QString selectLanguageKey("pacs/language");
const QString countImagesKey("pacs/pacsparam/countImages");
const QString prevImagesKey("pacs/pacsparam/prevImages");

#endif
