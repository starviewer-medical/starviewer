# En linux 64bits, de moment, breakpad no està suportat. Fem servir el següent codi per detectar quan
# estem compilant en aquesta plataforma ja que linux-g++-64 no ens serveix.

linux* {
    HARDWARE_PLATFORM = $$system(uname -m)
    contains( HARDWARE_PLATFORM, x86_64 ) {
        CONFIG += linux_x86_64
}
}