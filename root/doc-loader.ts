export class DocLoader {
    private static instances: Map<string, DocLoader> = new Map();
    private url: string;

    constructor(url: string) {
        this.url = url;
    }

    public static getInstance(url: string): DocLoader {
        if(!DocLoader.instances.has(url)) {
            if(!url) throw new Error('url is required');
            DocLoader.instances.set(url, new DocLoader(url));
        }
        return DocLoader.instances.get(url!)!;
    }

    /*
    ** Load
    */
    public async load(): Promise<Document | undefined> {
        try {
            const res = await fetch(this.url);
            if(!res.ok) throw new Error(`HTTP Error!, status: ${res.status}`);

            const html = await res.text();
            const parser = new DOMParser();
            const doc = parser.parseFromString(html, 'text/html');
            return doc;
        } catch(err) {
            console.error(err);
            return undefined;
        }
    }

    /*
    ** Set URL
    */
    public setUrl(url: string): void {
        this.url = url;
    }
}