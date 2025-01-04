import { Injectable } from '@angular/core';
import { HttpClient } from '@angular/common/http';
import { Observable, interval } from 'rxjs';
import { map, switchMap } from 'rxjs/operators';
import { SoilData } from '../models/soil-data.model';

@Injectable({
  providedIn: 'root'
})
export class SoilDataService {
  private apiUrl = 'http://localhost:5284/api/soilMoisture/Moisture'; // Replace with your actual API endpoint

  constructor(private http: HttpClient) {}

  getSoilMoistureStream(): Observable<SoilData> {
    // Poll the API every 5 seconds
    return interval(1000).pipe(
      switchMap(() => this.http.get<SoilData>(this.apiUrl))
    );
  }

  getSoilMoistureStreamTest(): Observable<SoilData> {
    // Simulate data for testing
    return interval(50000).pipe(
      map(() => ({
        timestamp: new Date(),
        moisturePercentage: Math.floor(Math.random() * 100),
        location: 'Garden Sensor 1'
      }))
    );
  }

}