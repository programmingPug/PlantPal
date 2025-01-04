import { TestBed } from '@angular/core/testing';

import { SoilDataService } from './soil-data.service';

describe('SoilDataService', () => {
  let service: SoilDataService;

  beforeEach(() => {
    TestBed.configureTestingModule({});
    service = TestBed.inject(SoilDataService);
  });

  it('should be created', () => {
    expect(service).toBeTruthy();
  });
});
