def defaultCMakeOptions =
    '-DCMAKE_BUILD_TYPE=Release ' +
    '-DOGS_LIB_BOOST=System ' +
    '-DOGS_LIB_VTK=System ' +
    '-DBUILD_SHARED_LIBS=ON ' +
    '-DOGS_BUILD_UTILS=ON '

def configure = new ogs.configure()
def build = new ogs.build()
def post = new ogs.post()
def helper = new ogs.helper()

stage('Configure (envinf1)') {
    configure.linux(cmakeOptions: defaultCMakeOptions, env: 'envinf1/cli.sh', script: this)
    configure.linux(cmakeOptions: defaultCMakeOptions + '-DOGS_USE_MPI=ON',
        dir: 'build-mpi', env: 'envinf1/mpi.sh', script: this)
}

stage('CLI (envinf1)') {
    parallel serial: {
        build.linux(env: 'envinf1/cli.sh', script: this)
    }, mpi: {
        build.linux(dir: 'build-mpi', env: 'envinf1/mpi.sh', script: this)
    }
}

stage('Test (envinf1)') {
    parallel serial: {
        build.linux(env: 'envinf1/cli.sh', script: this, target: 'tests ctest')
    }, mpi: {
        build.linux(dir: 'build-mpi', env: 'envinf1/mpi.sh', script: this,
            target: 'tests ctest')
    }
}

stage('Post (envinf1)') {
    post.publishTestReports 'build*/Testing/**/*.xml', 'build*/Tests/testrunner.xml',
        'ogs/scripts/jenkins/clang-log-parser.rules'
    post.cleanup()
}
