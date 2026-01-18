var Module = {
    locateFile: function(path, prefix) {
        if(path.endsWith('.data')) {
            return '../out/' + path;
        }
        if(path.endsWith('.wasm')) {
            return '../out/' + path;
        }
        return prefix + path;
    },
    preRun: [
        function() {
            console.log('preRun executing');
            
            const files = [
                'clouds.png',
                'rings.png'
            ];
            files.forEach(f => {
                const filePath = `./_resource/texture/${f}`;
                try {
                    FS.createPreloadedFile('/', f, filePath, true, false);
                    console.log(`Preloaded texture: ${f}`);
                } catch(error) {
                    console.warn(`Failed to preload texture: ${f}`, error);
                }
            });
        }
    ]
};