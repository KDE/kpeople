# Build the nie static lib here so the tests can use it too.
#nepomuk_add_ontology_classes (telepathynie_SRCS
#                              ONTOLOGIES
#                              ${SHAREDDESKTOPONTOLOGIES_ROOT_DIR}/nie/nie.trig
#                              ${SHAREDDESKTOPONTOLOGIES_ROOT_DIR}/nie/nfo.trig
#                              ${SHAREDDESKTOPONTOLOGIES_ROOT_DIR}/nie/nco.trig
#                              ${SHAREDDESKTOPONTOLOGIES_ROOT_DIR}/nie/nmo.trig
#                              ${CMAKE_SOURCE_DIR}/ontologies/telepathy.trig
#)

# Add the ontologies we want to build the vocabulary for.
soprano_add_ontology (telepathy_ontology_SRCS
                      ${CMAKE_SOURCE_DIR}/ontologies/telepathy.trig
                      "Telepathy"
                      "Nepomuk::Vocabulary"
                      "trig"
)

kde4_add_library (ontologies
                  STATIC
                  ${telepathynie_SRCS}
                  ${pimo_ontology_SRCS}
                  ${nco_ontology_SRCS}
                  ${nao_ontology_SRCS}
                  ${telepathy_ontology_SRCS}
)

set_property(TARGET ontologies PROPERTY COMPILE_FLAGS -fPIC)

