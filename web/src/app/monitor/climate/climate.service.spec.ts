import { TestBed } from '@angular/core/testing';

import { ClimateService } from './climate.service';

describe('ClimateService', () => {
  beforeEach(() => TestBed.configureTestingModule({}));

  it('should be created', () => {
    const service: ClimateService = TestBed.get(ClimateService);
    expect(service).toBeTruthy();
  });
});
