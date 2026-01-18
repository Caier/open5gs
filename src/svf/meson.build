# SVF stands for Security Vulnerability Function...

svf_sources = files('''
    sbi-path.c
    sbi-path.h
    event.c
    event.h
    svf-sm.c
    svf-sm.h
    nnrf-handler.c
    nnrf-handler.h
    shared.c
    shared.h

    app.c
    ../main.c
'''.split())

executable('open5gs-svfd',
    sources : svf_sources,
    c_args : '-DDEFAULT_CONFIG_FILENAME="@0@/svf.yaml"'.format(open5gs_sysconfdir),
    include_directories : srcinc,
    dependencies : libsbi_dep,
    install_rpath : libdir,
    install : true)

shared_library('svfd',
    sources: svf_sources,
    c_args : [
        '-DDEFAULT_CONFIG_FILENAME="@0@/svf.yaml"'.format(open5gs_sysconfdir),
        '-DBUILT_AS_DYLIB'
    ],
    include_directories : srcinc,
    dependencies : libsbi_dep
)