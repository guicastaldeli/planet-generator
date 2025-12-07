export class ClientColorConverter {
    private static instance: ClientColorConverter;

    public getInstance(): ClientColorConverter {
        if(!ClientColorConverter.instance) {
            ClientColorConverter.instance = new ClientColorConverter();
        }
        return ClientColorConverter.instance;
    }
}