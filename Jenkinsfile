#!/usr/bin/env groovy
@Library('jenkins-pipeline@1.0.22') _

def stage_required = [build: false, full: false]
def build_shared = 'ON'

pipeline {
  agent none
  options {
    ansiColor('xterm')
    timestamps()
    buildDiscarder(logRotator(numToKeepStr: '30', artifactNumToKeepStr: '10'))
    timeout(time: 6, unit: 'HOURS')
  }
  parameters {
    booleanParam(name: 'docker_conan', defaultValue: true)
    booleanParam(name: 'docker_conan_debug', defaultValue: true)
    booleanParam(name: 'docker_conan_gui', defaultValue: true)
    booleanParam(name: 'eve_serial', defaultValue: true)
    booleanParam(name: 'eve_parallel', defaultValue: true)
    booleanParam(name: 'win', defaultValue: true)
    booleanParam(name: 'mac', defaultValue: true)
    booleanParam(name: 'clang_analyzer', defaultValue: true)
    booleanParam(name: 'master_jobs', defaultValue: true)
  }
  stages {
     // *************************** Git Check **********************************
    stage('Pre-checks') {
      agent { label "master"}
      steps {
        sh "pre-commit install"
        sh "pre-commit run --all-files"
        sh "git config core.whitespace -blank-at-eof"
        sh "git diff --check `git merge-base origin/master HEAD` HEAD -- . ':!*.md' ':!*.pandoc' ':!*.asc' ':!*.dat' ':!*.ts'"
        dir('scripts/jenkins') { stash(name: 'known_hosts', includes: 'known_hosts') }
        ciSkip action: 'check' // Check for [ci skip] or [web] commit message.

        // ********* Check changesets for conditional stage execution **********
        script {
          def causes = currentBuild.getBuildCauses()
          for(cause in causes) {
            if (cause.class.toString().contains("UserIdCause")) {
              echo "Doing full build because job was started by user."
              stage_required.full = true
              env.CI_SKIP = "false"
              return true
            }
          }

          if (env.JOB_NAME == 'ufz/ogs/master') {
            build_shared = 'OFF'
          }
          if (currentBuild.number == 1 || buildingTag()) {
            stage_required.full = true
            return true
          }
          def changeLogSets = currentBuild.changeSets
          for (int i = 0; i < changeLogSets.size(); i++) {
            def entries = changeLogSets[i].items
            for (int j = 0; j < entries.length; j++) {
              def paths = new ArrayList(entries[j].affectedPaths)
              for (int p = 0; p < paths.size(); p++) {
                def path = paths[p]
                if (path.matches("Jenkinsfile")) {
                  stage_required.full = true
                  echo "Doing full build."
                  return true
                }
                if (path.matches("^(CMakeLists.txt|scripts|Applications|BaseLib|FileIO|GeoLib|MaterialLib|MathLib|MeshGeoToolsLib|MeshLib|NumLib|ProcessLib|SimpleTests|Tests).*")
                  && !stage_required.build) {
                  stage_required.build = true
                  echo "Doing regular build."
                }
              }
            }
          }
          if(!(stage_required.build || stage_required.full)) {
            currentBuild.result='NOT_BUILT'
          }
        }
      }
      // Mark build as NOT_BUILT when [ci skip] commit message was found
      post { always { ciSkip action: 'postProcess' } }
    }
    stage('Build') {
      parallel {
        // ************************ Docker-Conan *******************************
        stage('Docker-Conan') {
          when {
            beforeAgent true
            expression { return params.docker_conan && (stage_required.build || stage_required.full) }
          }
          agent {
            dockerfile {
              filename 'Dockerfile.gcc.full'
              dir 'scripts/docker'
              label 'docker'
              args '-v /home/jenkins/cache/ccache:/opt/ccache -v /home/jenkins/cache/conan/.conan:/opt/conan/.conan'
              additionalBuildArgs '--pull'
            }
          }
          environment {
            OMP_NUM_THREADS = '1'
            LD_LIBRARY_PATH = "$WORKSPACE/build/lib"
          }
          steps {
            script {
              sh 'git submodule sync'
              configure {
                cmakeOptions =
                  "-DBUILD_SHARED_LIBS=OFF " +
                  '-DOGS_CPU_ARCHITECTURE=generic ' +
                  '-DOGS_BUILD_UTILS=ON ' +
                  '-DOGS_CONAN_BUILD=missing ' +
                  '-DOGS_USE_CVODE=ON ' +
                  '-DOGS_USE_MFRONT=ON ' +
                  '-DOGS_USE_PYTHON=ON ' +
                  '-DOGS_BUILD_PROCESSES=HeatTransportBHE '
              }
              build {
                target="package"
                log="build1.log"
              }
              build { target="tests" }
              build { target="ctest" }
              build { target="doc" }
            }
          }
          post {
            always {
              xunit([
                // Testing/-folder is a CTest convention
                CTest(pattern: 'build/Testing/**/*.xml'),
                GoogleTest(pattern: 'build/Tests/testrunner.xml')
              ])
              recordIssues enabledForFailure: true, filters: [
                excludeFile('.*qrc_icons\\.cpp.*'), excludeFile('.*QVTKWidget.*'),
                excludeFile('.*\\.conan.*/TFEL/.*'),
                excludeFile('.*ThirdParty/MGIS/src.*'),
                excludeFile('.*MaterialLib/SolidModels/MFront/.*\\.mfront'),
                excludeFile('.*MaterialLib/SolidModels/MFront/.*\\.hxx'),
                excludeMessage('.*tmpnam.*')],
                tools: [gcc4(name: 'GCC', pattern: 'build/build*.log')],
                unstableTotalAll: 1
              recordIssues enabledForFailure: true, filters: [
                  excludeFile('-'), excludeFile('.*Functional\\.h'),
                  excludeFile('.*gmock-.*\\.h'), excludeFile('.*gtest-.*\\.h')
                ],
                // Doxygen is handled by gcc4 parser as well
                tools: [gcc4(name: 'Doxygen', id: 'doxygen',
                             pattern: 'build/DoxygenWarnings.log')],
                unstableTotalAll: 1
            }
            success {
              publishHTML(target: [allowMissing: false, alwaysLinkToLastBuild: true,
                  keepAll: true, reportDir: 'build/docs', reportFiles: 'index.html',
                  reportName: 'Doxygen'])
              archiveArtifacts 'build/*.tar.gz,build/conaninfo.txt'
              script {
                if (env.JOB_NAME == 'ufz/ogs/master') {
                  // Deploy Doxygen
                  unstash 'known_hosts'
                  sshagent(credentials: ['www-data_jenkins']) {
                    sh 'rsync -a --delete --stats -e "ssh -o UserKnownHostsFile=' +
                       'known_hosts" build/docs/. ' +
                       'www-data@jenkins.opengeosys.org:/var/www/doxygen.opengeosys.org'
                  }
                }
              }
            }
          }
        }
        // ************************** Windows **********************************
        stage('Win') {
          when {
            beforeAgent true
            expression { return params.win && (stage_required.build || stage_required.full) }
          }
          agent {label 'win && conan' }
          environment {
            MSVC_NUMBER = '15'
            MSVC_VERSION = '2017'
            OMP_NUM_THREADS = '1'
            CC = 'clcache'
            CXX = 'clcache'
          }
          steps {
            script {
              def num_threads = env.NUM_THREADS
              bat 'git submodule sync'
              bat 'conan remove --locks'
              configure { // CLI + GUI
                cmakeOptions =
                  "-DBUILD_SHARED_LIBS=OFF " +
                  '-DOGS_DOWNLOAD_ADDITIONAL_CONTENT=ON ' +
                  '-DOGS_BUILD_GUI=ON ' +
                  '-DOGS_BUILD_UTILS=ON ' +
                  '-DOGS_CONAN_BUILD=missing ' +
                  '-DOGS_BUILD_SWMM=ON ' +
                  '-DOGS_USE_NETCDF=ON ' +
                  '-DOGS_BUILD_PROCESSES=HeatTransportBHE '
              }
              build {
                target="package"
                log="build1.log"
                cmd_args="-l ${num_threads}"
              }
              configure { // CLI + GUI + Python
                cmakeOptions = '-DOGS_USE_PYTHON=ON '
                keepDir = true
              }
              build {
                target="package"
                log="build2.log"
              }
              build { target="tests" }
              build { target="ctest" }
            }
          }
          post {
            always {
              xunit([
                CTest(pattern: 'build/Testing/**/*.xml'),
                GoogleTest(pattern: 'build/Tests/testrunner.xml')
              ])
              recordIssues enabledForFailure: true, filters: [
                excludeFile('.*\\.conan.*'), excludeFile('.*ThirdParty.*'),
                excludeFile('.*thread.hpp')],
                tools: [msBuild(name: 'MSVC', pattern: 'build/build*.log')],
                qualityGates: [[threshold: 9, type: 'TOTAL', unstable: true]]
            }
            success {
              archiveArtifacts 'build/*.zip,build/conaninfo.txt'
            }
          }
        }
      } // end parallel
    } // end stage Build
  }
}
