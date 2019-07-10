#!/usr/bin/env groovy
@Library('jenkins-pipeline@1.0.22') _

pipeline {
  agent none
  options {
    ansiColor('xterm')
    timestamps()
    buildDiscarder(logRotator(numToKeepStr: '30', artifactNumToKeepStr: '10'))
    timeout(time: 6, unit: 'HOURS')
  }
  stages {
    stage('Build') {
      parallel {
        // ************************ Docker-Conan *******************************
        stage('Docker-Conan') {
          agent {
            dockerfile {
              filename 'Dockerfile.gcc.full'
              dir 'scripts/docker'
              label 'envinf1'
              args '-v /home/jenkins/cache/ccache:/opt/ccache -v /home/jenkins/cache/conan/.conan:/opt/conan/.conan'
              additionalBuildArgs '--pull'
            }
          }
          environment {
            OMP_NUM_THREADS = '1'
          }
          steps {
            script {
              sh 'git submodule sync'
              configure {
                cmakeOptions =
                  "-DBUILD_SHARED_LIBS=${build_shared} " +
                  '-DOGS_CPU_ARCHITECTURE=generic ' +
                  '-DOGS_BUILD_UTILS=ON ' +
                  '-DOGS_USE_CVODE=ON '
              }
              build { target="doc" }
            }
          }
          post {
            success {
              publishHTML(target: [allowMissing: false, alwaysLinkToLastBuild: true,
                  keepAll: true, reportDir: 'build/docs', reportFiles: 'index.html',
                  reportName: 'Doxygen'])
            }
          }
        }
      } // end parallel
    } // end stage Build
  }
}
