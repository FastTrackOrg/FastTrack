TEMPLATE = subdirs

SUBDIRS += \
    src/FastTrack.pro \
    src/FastTrack-Cli.pro \
    test/Test.pro

SKIP_TEST {
    SUBDIRS -= \
        test/Test.pro
}
