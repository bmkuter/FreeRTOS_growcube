export default {
    data() {
        return { arr: [{ field: 'air_delay_on', value: 0 }, { field: 'air_delay_off', value: 0 }, { field: '_delay_on', value: 0 }, { field: '_delay_off', value: 0 }, { field: '_pulse_width', value: 0 }, { field: 'source_delay_on', value: 0 }, { field: 'source_delay_off', value: 0 }, { field: 'source_pulse_width', value: 0 }, { field: 'drain_delay_on', value: 0 }, { field: 'drain_delay_off', value: 0 }, { field: 'drain_pulse_width', value: 0 }, { field: 'food_delay_on', value: 0 }, { field: 'food_delay_off', value: 0 }, { field: 'food_pulse_width', value: 0 }, { field: 'LED_delay_on', value: 0 }, { field: 'LED_delay_off', value: 0 }, { field: 'LED_pulse_width', value: 0 }] };
    },
    mounted() {
        console.log(this.arr.forEach);
        var req = new XMLHttpRequest();
        req.onreadystatechange = () => {
            if (req.readyState != 4) return; // State 4 is DONE
            const jsonResp = JSON.parse(req.responseText); //Get JSON so we can interact with the data
            this.arr.forEach(field => {
                let splitFields = field.field.split('_')
                let [root,subField] = [splitFields[0], splitFields.splice(1).join('_')]
                field.value = root ? jsonResp[root][subField] : jsonResp[subField];
            });
        };
        req.open("GET", "http://192.168.50.36:5000/query_device/2"); // Probably want to get the device ID programmatically
        req.send();
    },
    submit() {
        console.log("HTTP REQUEST GOES HERE");
    },
    template: `
    <div id="app">
        <form @submit.prevent="submit">
            <div v-for="item in arr" class="input-group mb-3">
                <span class="input-group-text" id="basic-addon3">{{item.field}}: </span>
                <input type="text" class="form-control" v-model="item.value">
            </div>
            <button @click="submit" class="btn btn-primary">Submit</button>
        </form>
    </div>`
}