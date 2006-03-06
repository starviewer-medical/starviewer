# Fitxer generat pel gestor de qmake de kdevelop. 
# ------------------------------------------- 
# Subdirectori relatiu al directori principal del projecte: .
# L'objectiu és un subdirectori del projecte 

SUBDIRS += src 
TEMPLATE = subdirs 
CONFIG += release \
          warn_on \
          qt \
          thread 
