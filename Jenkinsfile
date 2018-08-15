#!/usr/bin/env groovy
@Library('jenkins-pipeline@master') _

pipeline {
  agent { label 'win1' }
  options {
    ansiColor('xterm')
    timestamps()
    buildDiscarder(logRotator(numToKeepStr: '30', artifactNumToKeepStr: '10'))
    timeout(time: 3, unit: 'HOURS')
  }
  stages {
    stage('CMake') {
      steps {
        script {
          winDockerRun {
            cmd =
              "Remove-Item -Recurse -Force build; mkdir build; cd build; " +
              "cmake .. -G Ninja -DCMAKE_BUILD_TYPE=Release -DOGS_USE_CONAN=ON " +
              "-DOGS_BUILD_GUI=ON"
          }
        }
      }
    }
    stage('Build')
    {
      steps {
        script {
          winDockerRun { cmd = "cd build; ninja" }
        }
      }
    }
    stage('Test')
    {
      steps {
        script {
          winDockerRun { cmd = "cd build; ninja tests ctest"}
        }
      }
    }
  }
}
