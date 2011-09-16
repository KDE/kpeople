# Add the ontologies we want to build the vocabulary for.
soprano_add_ontology (pimo_ontology_SRCS
                      ${SHAREDDESKTOPONTOLOGIES_ROOT_DIR}/pimo/pimo.trig
                      "PIMO"
                      "Nepomuk::Vocabulary"
                      "trig"
)

soprano_add_ontology (nco_ontology_SRCS
                      ${SHAREDDESKTOPONTOLOGIES_ROOT_DIR}/nie/nco.trig
                      "NCO"
                      "Nepomuk::Vocabulary"
                      "trig"
)

soprano_add_ontology (nao_ontology_SRCS
                      ${SHAREDDESKTOPONTOLOGIES_ROOT_DIR}/nepomuk/nao.trig
                      "NAO"
                      "Nepomuk::Vocabulary"
                      "trig"
)

