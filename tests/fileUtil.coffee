log = new Duktape.Logger('fileUtil.coffee')
inf = () -> log.info.apply log, arguments
err = () -> log.error.apply log, arguments

throw new Error 'fileUtil package not loaded' if fileUtil == undefined

inf fileUtil.getFullPathName '/tmp'
