import { CommonModule } from '@angular/common';
import { Component } from '@angular/core';
import { SoilData } from '../models/soil-data.model';
import { deviceDto } from '../models/device-dto.model';
import { SoilDataService } from '../services/soil-data.service';
import { provideHttpClient, withInterceptorsFromDi } from '@angular/common/http';
import { MatDialog } from '@angular/material/dialog';
import { UpdateNicknameDialogComponent, UpdateNicknameData } from '../dialogs/update-nickname-dialog/update-nickname-dialog.component';

@Component({
  selector: 'app-plant-list',
  templateUrl: './plant-list.component.html',
  styleUrls: ['./plant-list.component.scss'],
  imports: [
    CommonModule
  ]
})

export class PlantListComponent {

  plants: SoilData[] = [];
  isLoading = true;

  constructor(
    private plantService: SoilDataService,
    private dialog: MatDialog,
  ) { }

  ngOnInit(): void {
    this.fetchPlants();
  }

  fetchPlants(): void {
    this.plantService.getPlants().subscribe({
      next: (data) => {
        this.plants = data;
        this.isLoading = false;
      },
      error: (err) => {
        console.error('Error fetching plants:', err);
        this.isLoading = false;
      },
    });
  }

  openUpdateDialog(device: SoilData): void {
    const dialogRef = this.dialog.open(UpdateNicknameDialogComponent, {
      width: '300px',
      data: { nickname: device.nickname }
    });

    dialogRef.afterClosed().subscribe(result => {
      if (result) {
        let deviceDto: deviceDto = {
          id: device.id,
          deviceId: device.deviceId,
          nickname: result.nickname
        };
        this.updateNickname(deviceDto);
      }
    });
  }

  updateNickname(deviceDto: deviceDto): void {
    this.plantService.updateNickname(deviceDto).subscribe(
      (response: any) => {
        this.fetchPlants();
      },
      (error: any) => {
        console.error('Error updating nickname:', error);
      }
    );
  }

}