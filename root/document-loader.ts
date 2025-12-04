export class DocumentLoader {
    private static instances: Map<string, DocumentLoader> = new Map();
    private url: string;

    constructor(url: string) {
        this.url = url;
    }

    public static getInstance(url: string): DocumentLoader {
        if(!DocumentLoader.instances.has(url)) {
            if(!url) throw new Error('url is required');
            DocumentLoader.instances.set(url, new DocumentLoader(url));
        }
        return DocumentLoader.instances.get(url!)!;
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