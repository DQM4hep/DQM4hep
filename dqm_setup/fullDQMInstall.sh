#!/bin/sh 

INSTALL=true

if [ -z "${ILCSOFT}" ]
 then
  echo "ILCSOFT not set"
  
 else echo "ILCSOFT is set to '${ILCSOFT}'"

  INSTALL_DIR="/opt/dqmsoftware"
  USER="acqilc"

  SOFT_LIST=(dqm4hep trivent calosoftware dqmsdhcal)

  # dqm4hep
  dqm4hep_GIT_REPO="dqm4hep"
  dqm4hep_CMAKE_CONFIG=(
  -DBUILD_DQMVIZ="ON"
  -DINSTALL_DOC="ON"
  -DBUILD_DQM4ILC="ON"
  -DDIM_GUI="ON"
  -DBUILD_EVB="ON"
  -DBUILD_HTTP="ON"
  -DUSE_MASTER="ON"
  )

  #trivent
  trivent_GIT_REPO="dqm4hep"
  trivent_CMAKE_CONFIG=()

  #calosoftware
  calosoftware_GIT_REPO="sdhcal"
  calosoftware_CMAKE_CONFIG=()

  # dqmsdhcal
  dqmsdhcal_GIT_REPO="sdhcal"
  DQMCore_DIR=$dqm4hep_DIR
  DQM4ILC_DIR=$dqm4hep_DIR
  xdrstream_DIR=$dqm4hep_DIR
  xdrlcio_DIR=$dqm4hep_DIR
  Trivent_DIR=$trivent_DIR
  CaloSoftWare_DIR=$calosoftware_DIR
  dqmsdhcal_CMAKE_CONFIG=(-DDQMCore_DIR=${DQMCore_DIR} -DDQM4ILC_DIR=${DQM4ILC_DIR} -Dxdrstream_DIR=${xdrstream_DIR} -Dxdrlcio_DIR=${xdrlcio_DIR} -DTrivent_DIR=${Trivent_DIR} -DCaloSoftWare_DIR=${CaloSoftWare_DIR})


# print recap
  for SOFT in ${SOFT_LIST[*]}; do
    echo " *** ${SOFT} will be installed in ${INSTALL_DIR}/${SOFT}"
    SOFT_DIR=$(echo ${SOFT}_DIR) 
    SOFT_DIR=${!SOFT_DIR} 

    GIT_REPO=$(echo ${SOFT}_GIT_REPO) 
    GIT_REPO=${!GIT_REPO} 
    echo -e "\tGIT_REPO = https://github.com/${GIT_REPO}/${SOFT}"
    
    CMAKE_CONFIG=$(echo ${SOFT}_CMAKE_CONFIG[@]) 
    CMAKE_CONFIG=${!CMAKE_CONFIG} 
    echo -e "\tcmake command: cmake -C ${ILCSOFT}/ILCSoft.cmake "${CMAKE_CONFIG}" .."
  done

if [ "$INSTALL" = true ]; then
    for SOFT in ${SOFT_LIST[*]}; do
      echo -e "\n\n *** installing ${SOFT} in ${INSTALL_DIR}/${SOFT}"
      # mkdir -p "${INSTALL_DIR}/${SOFT}"
      # sudo mkdir ${INSTALL_DIR}/${SOFT}
      # sudo chown ${USER}:${USER} ${INSTALL_DIR}/${SOFT}
      export ${SOFT}_DIR=${INSTALL_DIR}/${SOFT}
      SOFT_DIR=$(echo ${SOFT}_DIR) 
      SOFT_DIR=${!SOFT_DIR} 
      
      GIT_REPO=$(echo ${SOFT}_GIT_REPO) 
      GIT_REPO=${!GIT_REPO} 
      echo -e "\tGIT_REPO = https://github.com/${GIT_REPO}/${SOFT}"
      git clone https://github.com/$GIT_REPO/${SOFT} ${SOFT_DIR}
      mkdir -p "${SOFT_DIR}/build"
      cd ${SOFT_DIR}/build
      CMAKE_CONFIG=$(echo ${SOFT}_CMAKE_CONFIG[@]) 
      CMAKE_CONFIG=${!CMAKE_CONFIG} 
      echo -e "\tcmake command: cmake -C ${ILCSOFT}/ILCSoft.cmake "${CMAKE_CONFIG}" .."
      eval "cmake -C ${ILCSOFT}/ILCSoft.cmake "${CMAKE_CONFIG}" .."
      make
      make install
      cd -
    done
  fi
fi
