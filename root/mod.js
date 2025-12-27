var Module = {
    locateFile: function(path, prefix) {
        if(path.endsWith('.data')) {
            return '../out/' + path;
        }
        if(path.endsWith('.wasm')) {
            return '../out/' + path;
        }
        return prefix + path;
    }
}