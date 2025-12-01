export class DocumentLoader {
    private static instance: DocumentLoader;
    private url: string;

    constructor(url: string) {
        this.url = url;
    }

    public static getInstance(url: string): DocumentLoader {
        if(!DocumentLoader.instance) {
            if(!url) {
                throw new Error('URL is required!');
            }
            DocumentLoader.instance = new DocumentLoader(url);
        }
        if(url) {
            DocumentLoader.instance.setUrl(url);
        }
        return DocumentLoader.instance;
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