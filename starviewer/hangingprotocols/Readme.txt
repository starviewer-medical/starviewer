Explicació estructura directori hanging protocols
-------------------------------------------------

/hangingprotocols
   |
    -- Default          // En aquest directori hi va tot el que anirà a l'instal·lador per defecte
   |     |
   |     |
   |      -- Specific   // Aquí aniran HP que són específics d'algun lloc en concret però que no "fan mal" a la resta de llocs
   |     |     |
   |     |     |
   |     |      -- Site1
   |     |     |
   |     |     |
   |     |      -- Site1
   |     |     |
   |     |     |
   |     |      -- ...
   |     |
   |     |
   |      -- CT         //
   |     |
   |     |
   |      -- ...
   |
    -- Specific         // Directori on aniran els HP específics d'un lloc organitzats per lloc
   |     |              // A diferència de l'Specific de Default, aquests no s'instal·laran per defecte a tot arreu
   |     |              // i els haurà d'instal·lar a part l'administrador. També hi poden anar HP que els usuaris demanin personalitzats
   |     |
   |      -- Site1
   |     |     |
   |     |     |
   |     |      -- User1
   |     |
   |      -- Site2
   |     |
   |     |
   |      -- ...
   |
   |
    -- Testing          // Directori on aniran els HP de proves, test dels mateixos HP, etc.
