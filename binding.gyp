{
    "targets": [
        {
            "target_name": "ps-native",

            "sources": [
                "source/process.h",
				"source/helpers.h",
                "source/wrapper.h",
                "source/wrapper.cc",
                "source/binding.cc"
            ],

            "cflags": [
                "-std=c++11",
                "-stdlib=libc++"
            ],

            "include_dirs": [
                "<!(node -e \"require('nan')\")"
            ],

            "conditions": [
                ["OS == 'win'",
                    {
                        "defines": [
                            "PS_WIN32",
							"UNICODE",
							"_UNICODE"
                        ],

                        "sources": [
                            "source/process_win32.cc",
							"source/helpers_win32.cc"
                        ],

                        "msvs_settings": {
                            "VCCLCompilerTool": {
								"WarningLevel": 3,
                                "ExceptionHandling": 2,  # /EHsc
                            },
                        }
                    }
                 ],

                ["OS == 'linux'",
                    {
                        "defines": [
                            "PS_LINUX"
                        ],

                        "sources": [
                            "source/process_linux.cc",
							"source/helpers_linux.cc"
                        ]
                    }
                 ]
            ]
        }
    ]
}
