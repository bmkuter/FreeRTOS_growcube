// This file is the entirety of the app Component
// Vue components have data and lifecycle management functions
// Interact with the components only through lifecycle functions, never through the DOM e.g. getElementById and element.addClass("myClass")

// Should have more emits (not just a single update)
export default {
    props: ['system_name','delay_on','delay_off','pulse_width'],
    template: `
    <div id="system">
        <div class="input-group mb-3">
            <span class="input-group-text" id="basic-addon3">{{ this.system_name }}: Delay On</span>
            <input type="number" class="form-control" 
            :value="this.delay_on"
            v-on:paste="$emit('update', { system_name: this.system_name, update: 'delay_on', value: $event.target.value})"
            v-on:input="$emit('update', { system_name: this.system_name, update: 'delay_on', value: $event.target.value})">
        </div>
        <div class="input-group mb-3">
            <span class="input-group-text" id="basic-addon3">{{ this.system_name }}: Delay Off</span>
            <input type="number" class="form-control" 
            :value="this.delay_off"
            v-on:paste="$emit('update', { system_name: this.system_name, update: 'delay_off', value: $event.target.value})"
            v-on:input="$emit('update', { system_name: this.system_name, update: 'delay_off', value: $event.target.value})">
        </div>
        <div class="input-group mb-3">
            <span class="input-group-text" id="basic-addon3">{{ this.system_name }}: Pulse Width</span>
            <input type="number" class="form-control" 
            :value="this.pulse_width"
            v-on:paste="$emit('update', { system_name: this.system_name, update: 'pulse_width', value: $event.target.value})"
            v-on:input="$emit('update', { system_name: this.system_name, update: 'pulse_width', value: $event.target.value})">
        </div>  
    </div>`
}