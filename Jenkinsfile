#!/usr/bin/env groovy
@Library('jenkins-pipeline@1.0.9') _

def stage_required = [build: false, data: false, full: false]

pipeline {
  agent none
  options {
    ansiColor('xterm')
    timestamps()
    buildDiscarder(logRotator(numToKeepStr: '30', artifactNumToKeepStr: '10'))
    timeout(time: 3, unit: 'HOURS')
  }
  stages {
    // ******************** Deploy envinf1 PETSc ***************************
    stage('Deploy envinf1 PETSc') {
      agent { label "envinf1"}
      steps {
        script {
          configure {
            cmakeOptions =
              '-DOGS_USE_PETSC=ON ' +
              '-DOGS_BUILD_UTILS=ON ' +
              '-DBUILD_SHARED_LIBS=ON ' +
              '-DCMAKE_INSTALL_PREFIX=/global/apps/ogs/head/petsc ' +
              '-DOGS_MODULEFILE=/global/apps/modulefiles/ogs/head/petsc ' +
              '-DOGS_CPU_ARCHITECTURE=core-avx-i '
            env = 'envinf1/petsc.sh'
          }
          build {
            env = 'envinf1/petsc.sh'
            target = 'install'
            cmd_args = '-l 30'
          }
        }
      }
    }
  }
}
