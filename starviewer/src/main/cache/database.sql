-- ATENCIÓ IMPORTANT!!!!
-- CADA VEGADA QUE S'ACTUALITZI AQUEST SCRIPT, RECORDEU DE GENERAR UN SCRIPT PER ACTUALITZAR
-- LA BASE DE DADES EXISTENTS D'USUARIS D'STARVIEWER AMB ELS NOUS CANVIS I A ACTUALITZAR EL NUMERO 
-- DE REVISIO DE BASE DE DADES NECESSARIA PER LA COMPILACIÓ D'STARVIEWER ACTUA A core/starviewerapplication.h
-- A LA CONSTANT StarviewerDatabaseRevisionRequired

CREATE TABLE DatabaseRevision
(
  Revision TEXT
); 

-- Per actualitzar el número de revisió amb el del commit substituir $Rev$Rev$, la DatabaseRevision agafarà automàticament com a valor 
-- la revisió en que s'ha fet el commit amb el canvi a la base de dades.

INSERT INTO DatabaseRevision (Revision) VALUES ('$Rev$'); 

CREATE TABLE Patient
( 
  ID				TEXT PRIMARY KEY,
  Name	 			TEXT,
  BirthDate			TEXT,
  Sex				TEXT
);


CREATE TABLE Study
(
  InstanceUID			TEXT PRIMARY KEY,
  PatientID			TEXT NOT NULL,
  ID				TEXT NOT NULL,
  PatientAge			TEXT,
  PatientWeigth 		REAL,
  PatientHeigth 		REAL,
  Modalities			TEXT,
  Date				TEXT,
  Time				TEXT,
  AccessionNumber		TEXT,
  Description			TEXT,
  ReferringPhysicianName	TEXT,
  LastAccessDate		TEXT,
  RetrievedDate			TEXT,
  RetrievedTime			TEXT,
  State				INTEGER	
);

CREATE TABLE Series
(
  InstanceUID			TEXT PRIMARY KEY,
  StudyInstanceUID		TEXT,
  Number			TEXT,
  Modality			TEXT,
  Date				TEXT,
  Time				TEXT,
  InstitutionName		TEXT,
  PatientPosition		TEXT,
  ProtocolName			TEXT,
  Description			TEXT,
  FrameOfReferenceUID		TEXT,
  PositionReferenceIndicator	TEXT,
  BodyPartExaminated		TEXT,
  ViewPosition			TEXT,
  Manufacturer			TEXT,
  Laterality            TEXT,
  RetrievedDate			TEXT,
  RetrievedTime			TEXT,
  State				INTEGER
);

CREATE INDEX  IndexSeries_StudyInstanceUID ON Series (StudyInstanceUID); 

CREATE TABLE Image
(
  SOPInstanceUID		TEXT,
  FrameNumber           INTEGER,
  StudyInstanceUID		TEXT,
  SeriesInstanceUID		TEXT,
  InstanceNumber		TEXT,
  ImageOrientationPatient	TEXT,
  PatientOrientation		TEXT,
  PixelSpacing			TEXT,
  SliceThickness		REAL,
  PatientPosition		TEXT,
  SamplesPerPixel		INTEGER,
  Rows				INTEGER,
  Columns			INTEGER,
  BitsAllocated 		INTEGER,
  BitsStored			INTEGER,
  PixelRepresentation		INTEGER,
  RescaleSlope			REAL,
  WindowLevelWidth		TEXT,
  WindowLevelCenter		TEXT,
  WindowLevelExplanations	TEXT,
  SliceLocation 		TEXT,
  RescaleIntercept		REAL,
  PhotometricInterpretation	TEXT,
  ImageType             TEXT,
  ViewPosition          TEXT,
  ImageLaterality       TEXT,
  ViewCodeMeaning       TEXT,
  PhaseNumber           INTEGER,
  ImageTime           TEXT,
  VolumeNumberInSeries  INTEGER,
  OrderNumberInVolume		INTEGER,
  RetrievedDate			TEXT,
  RetrievedTime			TEXT,
  State				INTEGER ,
  PRIMARY KEY ('SOPInstanceUID', 'FrameNumber')
);

--TODO:Comprovar si s'utilitzarà l'index IndexImage_StudyInstanceUIDSeriesInstanceUID després dels canvis fets a la BD
CREATE INDEX  IndexImage_StudyInstanceUIDSeriesInstanceUID ON Image (StudyInstanceUID,SeriesInstanceUID); 
CREATE INDEX  IndexImage_SOPInstanceUIDOrderNumberInVolume ON Image (SOPInstanceUID, OrderNumberInVolume); 