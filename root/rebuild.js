let lastBuildCheck = null;
        
setInterval(async () => {
    try {
        const response = await fetch('../out/rebuild.txt?t=' + Date.now(), {
            cache: 'no-store'
        });
                
        if(response.ok) {
            const buildTime = await response.text();
            
            if(lastBuildCheck !== null && buildTime !== lastBuildCheck) {
                console.log('New build detected, reloading...');
                location.reload(true);
            }
            
            lastBuildCheck = buildTime;
        }
    } catch(err) {
        console.log(err);
    }
}, 1000);