<?xml version="1.0" encoding="utf-8"?>
<!DOCTYPE TS>
<TS version="2.1" language="es_US">
<context>
    <name>QObject</name>
    <message>
        <location filename="../src/main.cpp" line="47"/>
        <source>Optional filename to write configuration to. Package name will be used if not specified.</source>
        <translation>Nombre de archivo opcional donde escribir la configuración. El nombre del paquete se usará si no se especifica.</translation>
    </message>
    <message>
        <location filename="../src/main.cpp" line="53"/>
        <source>Write given keyword to Portage&apos;s keywords folder. Pass - for ~amd64.</source>
        <translation>Escribir el keyword dado a la carpeta de keywords de Portage. Pasa - para ~amd64.</translation>
    </message>
    <message>
        <location filename="../src/main.cpp" line="59"/>
        <source>Write given license to Portage&apos;s licenses folder.</source>
        <translation>Escribir la licencia dada a la carpeta de licencias de Portage.</translation>
    </message>
    <message>
        <location filename="../src/main.cpp" line="65"/>
        <source>Mask given package.</source>
        <translation>Enmascara el paquete dado.</translation>
    </message>
    <message>
        <source>Write given keyword to Portage&apos;s keywords folder. ~amd64 will be written if none is specified.</source>
        <translation type="vanished">Escribe el keyword dado a la carpeta de keywords de Portage. ~amd64 será escrito si no se especifica ninguno con esta opción.</translation>
    </message>
    <message>
        <location filename="../src/main.cpp" line="33"/>
        <source>Write given env config to Portage&apos;s global env folder.</source>
        <translation>Escribe la configuración env dada a la carpeta env global de Portage.</translation>
    </message>
    <message>
        <location filename="../src/main.cpp" line="39"/>
        <source>Write given env config to Portage&apos;s package env folder.
Note that given env config should exist in /etc/portage/env for portage to work as expected!</source>
        <translation>Escribir la configuración env dada a la carpeta env para paquetes de Portage.
¡Toma en cuenta que la configuración env dada debería existir en /etc/portage/env para que portage funcione como se espera!</translation>
    </message>
    <message>
        <location filename="../src/main.cpp" line="60"/>
        <source>licenses</source>
        <translation>licencias</translation>
    </message>
    <message>
        <location filename="../src/main.cpp" line="70"/>
        <source>The package in question. This option is mandatory.</source>
        <translation>El paquete en cuestión. Esta opción es obligatoria.</translation>
    </message>
    <message>
        <location filename="../src/main.cpp" line="75"/>
        <source>Write given USE flags to Portage&apos;s USE flags folder.</source>
        <translation>Escribe las USE flags dadas a la carpeta de USE flags de Portage.</translation>
    </message>
    <message>
        <location filename="../src/main.cpp" line="81"/>
        <source>Unmask given package.</source>
        <translation>Desenmascara el paquete dado.</translation>
    </message>
    <message>
        <location filename="../src/main.cpp" line="86"/>
        <source>Describe every given step.</source>
        <translation>Describir cada paso dado.</translation>
    </message>
    <message>
        <location filename="../src/main.cpp" line="102"/>
        <source>Superuser rights needed. Portage&apos;s directories belong to root. Can&apos;t write there as %1.</source>
        <translation>Se necesitan permisos de super usuario. Las carpetas de Portage pertenecen a root. No se puede escribir ahí como %1.</translation>
    </message>
    <message>
        <location filename="../src/main.cpp" line="110"/>
        <source>Package wasn&apos;t provided.</source>
        <translation>El paquete no fue especificado.</translation>
    </message>
    <message>
        <location filename="../src/main.cpp" line="115"/>
        <source>Both mask and unmask options were provided where just one at a time is allowed.</source>
        <translation>Tanto mask como unmask fueron especificadas donde sólo una a la vez está permitido.</translation>
    </message>
    <message>
        <location filename="../src/main.cpp" line="125"/>
        <source>Filename must be provided.</source>
        <translation>El nombre del archivo debe ser especificado.</translation>
    </message>
</context>
<context>
    <name>Writer</name>
    <message>
        <location filename="../src/writer.cpp" line="31"/>
        <source>Values weren&apos;t provided for package: %1</source>
        <translation>Los valores no fueron especificados para el paquete: %1</translation>
    </message>
    <message>
        <location filename="../src/writer.cpp" line="39"/>
        <source>Directory: %1 doesn&apos;t exist. Creating it...</source>
        <translation>La carpeta: %1 no existe. Creándola...</translation>
    </message>
    <message>
        <location filename="../src/writer.cpp" line="45"/>
        <source>Using package name as file name.</source>
        <translation>Usando el nombre del paquete como nombre para el archivo.</translation>
    </message>
    <message>
        <location filename="../src/writer.cpp" line="57"/>
        <source>Writing %1&apos;%2&apos;%3 to %4%5%6.</source>
        <translation>Escribiendo %1&apos;%2&apos;%3 a %4%5%6.</translation>
    </message>
    <message>
        <location filename="../src/writer.cpp" line="63"/>
        <source>Could not open file: &apos;%1&apos; to write config to.</source>
        <translation>No se pudo abrir el archiv: &apos;%1&apos; para escribir la configuración.</translation>
    </message>
</context>
</TS>
