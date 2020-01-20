from distutils.core import setup, Extension

def main():
    setup(name="client",
          version="1.0.0",
          description="Python interface for the client",
          author="Zuzanna Piniarska Mateusz Kalamoniak",
          author_email="zuza.piniarska0910@gmail.com",
          ext_modules=[Extension("client", ["main2.cpp"])])

if __name__ == "__main__":
    main()
