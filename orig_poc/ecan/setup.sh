export install_path="/opt/e17"
        echo -n "- setting env variables ...... "
        export PATH="$install_path/bin:$PATH"
        export ACLOCAL_FLAGS="-I $install_path/share/aclocal $ACLOCAL_FLAGS"
        export LD_LIBRARY_PATH="$install_path/lib:$LD_LIBRARY_PATH"
        export PKG_CONFIG_PATH="$install_path/lib/pkgconfig:$PKG_CONFIG_PATH"
        export CPPFLAGS="$CPPFLAGS -I$install_path/include"
        export LDFLAGS="$LDFLAGS -L$install_path/lib"
        export CFLAGS="$CFLAGS"
        export PYTHONPATH=`python -c "import distutils.sysconfig; print distutils.sysconfig.get_python_lib(prefix='$install_path')" 2>/dev/null`
        export PYTHONINCLUDE=`python -c "import distutils.sysconfig; print distutils.sysconfig.get_python_inc(prefix='$install_path')" 2>/dev/null`
        echo "ok"
gcc hello.c -o hello `pkg-config elementary --cflags --libs`
