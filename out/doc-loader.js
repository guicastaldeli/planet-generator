export class DocLoader {
    constructor(url) {
        this.url = url;
    }
    setUrl(url) {
        this.url = url;
    }
    static getInstance(url) {
        if (!DocLoader.instances.has(url)) {
            if (!url)
                throw new Error('url is required');
            DocLoader.instances.set(url, new DocLoader(url));
        }
        return DocLoader.instances.get(url);
    }
    /**
     * Load
     */
    async load() {
        try {
            const res = await fetch(this.url);
            if (!res.ok)
                throw new Error(`HTTP Error!, status: ${res.status}`);
            const html = await res.text();
            const parser = new DOMParser();
            const doc = parser.parseFromString(html, 'text/html');
            return doc;
        }
        catch (err) {
            console.error(err);
            return undefined;
        }
    }
}
DocLoader.instances = new Map();
