export interface ClimateSample {
    temperature: number;
    humidity: number;
}

export interface ClimateSamples {
    timestamps: number[];
    samples: ClimateSample[];
}

export interface ClimateData {
    interior: ClimateSamples;
    exterior: ClimateSamples;
}
