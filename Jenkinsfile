#!/usr/bin/env groovy
@Library('jenkins-pipeline@1.0.9') _

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
        bat '''
          docker run --rm -v %cd%:C:\\jenkins-ws -w C:\\jenkins-ws \
            --mount source=cache-conan-short,destination=C:\\.conan \
            --mount source=cache-conan,destination=C:\\conan-cache \
            registry.opengeosys.org/ogs/ogs/msvc/2017:latest \
            powershell -NoLogo -ExecutionPolicy Bypass; \
            $env:CONAN_USER_HOME = 'C:\\conan-cache'; \
            Remove-Item -Recurse -Force build; mkdir build; cd build; \
            cmake .. -G Ninja -DCMAKE_BUILD_TYPE=Release -DOGS_USE_CONAN=ON \
            -DOGS_BUILD_GUI=ON
        '''
      }
    }
    stage('Build')
    {
      steps {
        bat '''
        docker run --rm -v %cd%:C:\\jenkins-ws -w C:\\jenkins-ws \
          --mount source=cache-conan-short,destination=C:\\.conan \
          --mount source=cache-conan,destination=C:\\conan-cache \
          registry.opengeosys.org/ogs/ogs/msvc/2017:latest \
          powershell -NoLogo -ExecutionPolicy Bypass; \
          $env:CONAN_USER_HOME = 'C:\\conan-cache'; \
          cd build; ninja
        '''
      }
    }
    stage('Test')
    {
      steps {
        bat '''
        docker run --rm -v %cd%:C:\\jenkins-ws -w C:\\jenkins-ws \
          --mount source=cache-conan-short,destination=C:\\.conan \
          --mount source=cache-conan,destination=C:\\conan-cache \
          registry.opengeosys.org/ogs/ogs/msvc/2017:latest \
          powershell -NoLogo -ExecutionPolicy Bypass; \
          $env:CONAN_USER_HOME = 'C:\\conan-cache'; \
          cd build; ninja tests ctest
        '''
      }
    }
  }
}
