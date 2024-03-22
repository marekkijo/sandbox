import os
from conan import ConanFile
from conan.tools.cmake import CMakeToolchain, CMake, cmake_layout, CMakeDeps
from conan.tools.files import patch


class libdatachannelRecipe(ConanFile):
    name = 'libdatachannel'
    version = '0.20.2'
    package_type = 'library'

    license = 'Mozilla Public License Version 2.0'
    author = 'Paul-Louis Ageneau'
    url = 'https://github.com/paullouisageneau/libdatachannel'
    description = 'C/C++ WebRTC network library featuring Data Channels, Media Transport, and WebSockets'
    topics = ('c-plus-plus', 'cpp', 'websocket', 'webrtc', 'peer-to-peer', 'p2p', 'datachannel', 'sctp', 'webrtc-video', 'libnice', 'peerconnection', 'webrtc-datachannel', 'rtcpeerconnection', 'rtcdatachannel', 'libdatachannel', 'rfc-8831', 'rfc-8834')
    settings = 'os', 'compiler', 'build_type', 'arch'
    options = {
        'shared': [True, False],
        'fPIC': [True, False],

        'USE_GNUTLS': [True, False],   # Use GnuTLS instead of OpenSSL
        'USE_MBEDTLS': [True, False],  # Use Mbed TLS instead of OpenSSL
        'USE_NICE': [True, False],     # Use libnice instead of libjuice
        'NO_WEBSOCKET': [True, False], # Disable WebSocket support
        'NO_MEDIA': [True, False],     # Disable media transport support
        'CAPI_STDCALL': [True, False], # Set calling convention of C API callbacks stdcall
        'SCTP_DEBUG': [True, False]    # Enable SCTP debugging output to verbose log
    }
    default_options = {
        'shared': False,
        'fPIC': True,

        'USE_GNUTLS': False,
        'USE_MBEDTLS': False,
        'USE_NICE': False,
        'NO_WEBSOCKET': False,
        'NO_MEDIA': False,
        'CAPI_STDCALL': False,
        'SCTP_DEBUG': False
    }
    exports_sources = 'patches/*.patch'

    def config_options(self):
        if self.settings.os == 'Windows':
            del self.options.fPIC

    def configure(self):
        if self.options.shared:
            self.options.rm_safe('fPIC')

    def layout(self):
        cmake_layout(self, src_folder='src')

    def requirements(self):
        self.requires('openssl/3.2.1')

    def source(self):
        self.run('git clone https://github.com/paullouisageneau/libdatachannel.git ' + self.source_folder)
        self.run('git -C ' + self.source_folder + ' checkout v' + self.version)
        self.run('git -C ' + self.source_folder + ' submodule update --init --recursive --depth 1')
        patch_file = os.path.join(self.export_sources_folder, 'patches/static-no-exclude-for-all.patch')
        patch(self, patch_file=patch_file)

    def generate(self):
        deps = CMakeDeps(self)
        deps.generate()
        tc = CMakeToolchain(self)
        tc.cache_variables['USE_GNUTLS'] = self.options.USE_GNUTLS
        tc.cache_variables['USE_MBEDTLS'] = self.options.USE_MBEDTLS
        tc.cache_variables['USE_NICE'] = self.options.USE_NICE
        tc.cache_variables['PREFER_SYSTEM_LIB'] = False
        tc.cache_variables['NO_WEBSOCKET'] = self.options.NO_WEBSOCKET
        tc.cache_variables['NO_MEDIA'] = self.options.NO_MEDIA
        tc.cache_variables['NO_EXAMPLES'] = True
        tc.cache_variables['NO_TESTS'] = True
        tc.cache_variables['WARNINGS_AS_ERRORS'] = True
        tc.cache_variables['CAPI_STDCALL'] = self.options.CAPI_STDCALL
        tc.cache_variables['SCTP_DEBUG'] = self.options.SCTP_DEBUG
        tc.generate()

    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()

    def package(self):
        cmake = CMake(self)
        cmake.install()

    def package_info(self):
        self.cpp_info.components['LibDataChannel'].set_property('cmake_target_name', 'LibDataChannel::LibDataChannel')
        self.cpp_info.components['LibDataChannel'].set_property('pkg_config_name', 'LibDataChannel')
        self.cpp_info.components['LibDataChannel'].libs = ['datachannel']
        self.cpp_info.components['LibDataChannelStatic'].set_property('cmake_target_name', 'LibDataChannel::LibDataChannelStatic')
        self.cpp_info.components['LibDataChannelStatic'].set_property('pkg_config_name', 'LibDataChannelStatic')
        self.cpp_info.components['LibDataChannelStatic'].libs = ['datachannel-static']
