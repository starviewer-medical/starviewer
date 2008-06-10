# Fitxer generat pel gestor de qmake de kdevelop. 
# ------------------------------------------- 
# Subdirectori relatiu al directori principal del projecte: .
# L'objectiu és un subdirectori del projecte 

include(src/compilationtype.inc)

SUBDIRS += src 
TEMPLATE = subdirs 
CONFIG += warn_on \
          qt \
          thread 
